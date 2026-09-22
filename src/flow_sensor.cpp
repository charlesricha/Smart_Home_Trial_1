/**
 * @file flow_sensor.cpp
 * @brief Implementation of pulse flow sensor driver with debounced ISR and NVS persistence
 */

#include "flow_sensor.h"
#include <Preferences.h>

FlowSensor flowSensor;

static Preferences prefs;
static portMUX_TYPE isrMux = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t isr_pulse_count = 0;
static volatile uint32_t isr_last_pulse_us = 0;

void IRAM_ATTR isr_flow_pulse() {
    uint32_t now = (uint32_t)micros();
    // Hardware glitch debounce check
    if ((now - isr_last_pulse_us) >= MIN_PULSE_INTERVAL_US) {
        portENTER_CRITICAL_ISR(&isrMux);
        isr_pulse_count++;
        isr_last_pulse_us = now;
        portEXIT_CRITICAL_ISR(&isrMux);
    }
}

FlowSensor::FlowSensor()
    : _flow_rate_lpm(0.0f),
      _session_volume_l(0.0f),
      _total_volume_l(0.0f),
      _nvs_committed_vol_l(0.0f),
      _last_sample_ms(0),
      _last_pulse_sample_count(0),
      _flow_stop_ms(0),
      _session_start_ms(0),
      _session_duration_s(0),
      _is_flowing(false),
      _nvs_dirty(false) {}

void FlowSensor::begin() {
    // Configure flow sensor input pin with pullup (open-collector Hall sensors require pullup)
    pinMode(PIN_FLOW_SENSOR, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_FLOW_SENSOR), isr_flow_pulse, RISING);

    // Initialize NVS and load persistent total volume
    prefs.begin(NVS_NAMESPACE, false);
    _total_volume_l = prefs.getFloat(NVS_KEY_TOTAL_VOL, 0.0f);
    _nvs_committed_vol_l = _total_volume_l;

    Serial.printf("[FLOW] Initialized on GPIO %d. Lifetime volume loaded: %.2f L\n",
                  PIN_FLOW_SENSOR, _total_volume_l);

    _last_sample_ms = millis();
}

void FlowSensor::update(uint32_t now_ms) {
    if (now_ms - _last_sample_ms < FLOW_SAMPLE_PERIOD_MS) {
        return;
    }

    uint32_t dt_ms = now_ms - _last_sample_ms;
    _last_sample_ms = now_ms;

    // Atomically read the ISR pulse counter
    uint32_t current_pulses;
    portENTER_CRITICAL(&isrMux);
    current_pulses = isr_pulse_count;
    portEXIT_CRITICAL(&isrMux);

    uint32_t delta_pulses = current_pulses - _last_pulse_sample_count;
    _last_pulse_sample_count = current_pulses;

    // Calculate instantaneous flow rate: Q (L/min) = (Hz) / FLOW_CALIBRATION_FACTOR
    float dt_sec = (float)dt_ms / 1000.0f;
    float pulse_freq_hz = (float)delta_pulses / dt_sec;
    float raw_flow_lpm = pulse_freq_hz / FLOW_CALIBRATION_FACTOR;

    // Detect flow activity and session timing
    if (delta_pulses > 0) {
        if (!_is_flowing) {
            _is_flowing = true;
            _session_start_ms = now_ms;
        }
        _session_duration_s = (now_ms - _session_start_ms) / 1000;
        _flow_stop_ms = 0;

        // Exponential moving average filter for smooth readout
        _flow_rate_lpm = (_flow_rate_lpm * 0.4f) + (raw_flow_lpm * 0.6f);
        Serial.printf("[FLOW] Pulses: %u (%.1f Hz) -> Flow: %.2f L/min | Session: %.2f L\n",
                      delta_pulses, pulse_freq_hz, _flow_rate_lpm, _session_volume_l);

        // Integrate volume
        float delta_liters = (float)delta_pulses / PULSES_PER_LITER;
        _session_volume_l += delta_liters;
        _total_volume_l += delta_liters;
        _nvs_dirty = true;
    } else {
        // No pulses in this sampling window
        _flow_rate_lpm = 0.0f;
        if (_is_flowing) {
            if (_flow_stop_ms == 0) {
                _flow_stop_ms = now_ms;
            } else if (now_ms - _flow_stop_ms > 1200) {
                // Flow has ceased
                _is_flowing = false;
                _session_duration_s = 0;
            }
        }
    }

    // NVS Wear-Leveling persistence logic
    // Rule 1: Commit when delta >= 1.0 Liter
    if ((_total_volume_l - _nvs_committed_vol_l) >= NVS_VOL_COMMIT_DELTA_L) {
        forceCommitNvs();
    }
    // Rule 2: Commit when flow stops after being active
    else if (_nvs_dirty && !_is_flowing && _flow_stop_ms > 0 &&
             (now_ms - _flow_stop_ms >= NVS_IDLE_COMMIT_DELAY_MS)) {
        forceCommitNvs();
    }
}

void FlowSensor::resetSession() {
    _session_volume_l = 0.0f;
    _session_duration_s = 0;
    _session_start_ms = millis();
}

void FlowSensor::resetTotal() {
    _total_volume_l = 0.0f;
    forceCommitNvs();
    Serial.println("[FLOW] Lifetime total volume reset to 0.00 L");
}

void FlowSensor::forceCommitNvs() {
    prefs.putFloat(NVS_KEY_TOTAL_VOL, _total_volume_l);
    _nvs_committed_vol_l = _total_volume_l;
    _nvs_dirty = false;
    Serial.printf("[FLOW] NVS committed: Total Volume = %.2f L\n", _total_volume_l);
}
