/**
 * @file flow_sensor.h
 * @brief Hall-effect pulse flow sensor driver with glitch-debounced IRAM ISR,
 *        real-time flow/volume calculations, and NVS wear-leveling persistence.
 */

#pragma once

#include <Arduino.h>
#include "config.h"

class FlowSensor {
public:
    FlowSensor();

    /**
     * @brief Configure GPIO, attach IRAM interrupt, and load persistent volume from NVS
     */
    void begin();

    /**
     * @brief Periodic calculation function called from the control task loop
     * @param now_ms Current millis() timestamp
     */
    void update(uint32_t now_ms);

    /**
     * @brief Reset the active session volume counter (e.g., when valve opens)
     */
    void resetSession();

    /**
     * @brief Reset lifetime total volume in RAM and commit to NVS
     */
    void resetTotal();

    /**
     * @brief Force commit current total volume to NVS (e.g. before shutdown/sleep)
     */
    void forceCommitNvs();

    // Getters
    float getInstantFlowRate() const { return _flow_rate_lpm; }
    float getSessionVolume() const { return _session_volume_l; }
    float getTotalVolume() const { return _total_volume_l; }
    bool  isFlowing() const { return _is_flowing; }
    uint32_t getSessionDurationSec() const { return _session_duration_s; }
    uint32_t getRawPulseCount() const;
    uint8_t  getPinLevel() const;


private:
    float _flow_rate_lpm;
    float _session_volume_l;
    float _total_volume_l;
    float _nvs_committed_vol_l;

    uint32_t _last_sample_ms;
    uint32_t _last_pulse_sample_count;
    uint32_t _flow_stop_ms;
    uint32_t _session_start_ms;
    uint32_t _session_duration_s;

    bool _is_flowing;
    bool _nvs_dirty;
};

extern FlowSensor flowSensor;
