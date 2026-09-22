/**
 * @file safety_supervisor.cpp
 * @brief Implementation of safety supervisor and autonomous flood prevention
 */

#include "safety_supervisor.h"

SafetySupervisor safetySupervisor;

SafetySupervisor::SafetySupervisor()
    : _active_alarm(SafetyAlarm::NONE),
      _leak_detection_start_ms(0) {}

void SafetySupervisor::begin() {
    Serial.println("[SAFETY] Supervisor active. Armed with continuous flow & leak detection.");
}

void SafetySupervisor::triggerEmergencyCutoff(SafetyAlarm alarm) {
    _active_alarm = alarm;
    valveController.close();

    const char* reason = "UNKNOWN";
    if (alarm == SafetyAlarm::MAX_TIME_EXCEEDED) reason = "CONTINUOUS FLOW TIMEOUT EXCEEDED (15m)";
    else if (alarm == SafetyAlarm::MAX_VOLUME_EXCEEDED) reason = "SESSION VOLUME QUOTA EXCEEDED (50L)";
    else if (alarm == SafetyAlarm::LEAK_DETECTED) reason = "FLOW DETECTED WHILE VALVE CLOSED (LEAK)";

    Serial.printf("\n========================================================\n");
    Serial.printf(" [SAFETY ALERT] EMERGENCY SHUTOFF TRIGGERED!\n");
    Serial.printf(" REASON: %s\n", reason);
    Serial.printf("========================================================\n\n");
}

void SafetySupervisor::resetAlarm() {
    Serial.println("[SAFETY] Alarm cleared by user.");
    _active_alarm = SafetyAlarm::NONE;
    _leak_detection_start_ms = 0;
}

void SafetySupervisor::update(uint32_t now_ms) {
    // 1. Continuous Flow Timeout Check (Flood Protection)
    if (valveController.isOpen() && flowSensor.isFlowing()) {
        if (flowSensor.getSessionDurationSec() >= SAFETY_MAX_CONTINUOUS_SEC) {
            triggerEmergencyCutoff(SafetyAlarm::MAX_TIME_EXCEEDED);
            return;
        }
    }

    // 2. Maximum Session Volume Check (Quota Protection)
    if (valveController.isOpen()) {
        if (flowSensor.getSessionVolume() >= SAFETY_MAX_SESSION_VOL_L) {
            triggerEmergencyCutoff(SafetyAlarm::MAX_VOLUME_EXCEEDED);
            return;
        }
    }

    // 3. Unintended Flow / Pipe Leak Detection Check
    // If valve is commanded CLOSED (and settled), but flow is still detected
    if (valveController.getState() == ValveState::CLOSED) {
        if (flowSensor.getInstantFlowRate() >= SAFETY_LEAK_THRESH_LPM) {
            if (_leak_detection_start_ms == 0) {
                _leak_detection_start_ms = now_ms;
            } else if (now_ms - _leak_detection_start_ms >= 2500) { // 2.5s persistent flow
                if (_active_alarm != SafetyAlarm::LEAK_DETECTED) {
                    _active_alarm = SafetyAlarm::LEAK_DETECTED;
                    Serial.printf("[SAFETY ALERT] Leak detected! Flow = %.2f L/min with valve closed.\n",
                                  flowSensor.getInstantFlowRate());
                }
            }
        } else {
            // Transient or no flow
            _leak_detection_start_ms = 0;
        }
    } else {
        _leak_detection_start_ms = 0;
    }
}
