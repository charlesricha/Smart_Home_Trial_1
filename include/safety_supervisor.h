/**
 * @file safety_supervisor.h
 * @brief Flood protection supervisor: continuous flow cutoff, volume quotas, leak detection
 */

#pragma once

#include <Arduino.h>
#include "config.h"
#include "types.h"
#include "valve_controller.h"
#include "flow_sensor.h"

class SafetySupervisor {
public:
    SafetySupervisor();

    void begin();

    /**
     * @brief Periodic safety evaluation loop running in the Control Task
     * @param now_ms Current millis() timestamp
     */
    void update(uint32_t now_ms);

    /**
     * @brief Acknowledge and clear current safety alarm
     */
    void resetAlarm();

    /**
     * @brief Trigger an immediate emergency valve closure
     * @param alarm The alarm reason
     */
    void triggerEmergencyCutoff(SafetyAlarm alarm);

    SafetyAlarm getActiveAlarm() const { return _active_alarm; }
    bool isAlarmActive() const { return _active_alarm != SafetyAlarm::NONE; }

private:
    SafetyAlarm _active_alarm;
    uint32_t    _leak_detection_start_ms;
};

extern SafetySupervisor safetySupervisor;
