/**
 * @file types.h
 * @brief Common types, structures, and enumerations for Smart Home Water Node
 */

#pragma once
#include <Arduino.h>

// Valve operational states
enum class ValveState : uint8_t {
    CLOSED = 0,
    OPEN = 1,
    MOVING_TO_OPEN = 2,
    MOVING_TO_CLOSE = 3
};

// System safety alarm flags
enum class SafetyAlarm : uint8_t {
    NONE = 0,
    LEAK_DETECTED = 1,          // Flow detected while valve commanded CLOSED
    MAX_TIME_EXCEEDED = 2,      // Open continuous time limit exceeded
    MAX_VOLUME_EXCEEDED = 3,    // Open session volume limit exceeded
    HARDWARE_FAULT = 4
};

// Inter-task commands sent to the Control Task
enum class ControlCommandType : uint8_t {
    CMD_VALVE_TOGGLE = 0,
    CMD_VALVE_OPEN = 1,
    CMD_VALVE_CLOSE = 2,
    CMD_RESET_ALARM = 3,
    CMD_RESET_VOLUME = 4
};

struct ControlCommand {
    ControlCommandType type;
    uint32_t param;
};

// Snapshot of system telemetry shared between Control and UI tasks
struct SystemTelemetry {
    ValveState valve_state;
    SafetyAlarm safety_alarm;
    float flow_rate_lpm;        // Instantaneous flow rate (L/min)
    float session_volume_l;     // Current session volume (Liters)
    float total_volume_l;       // Lifetime persistent volume (Liters)
    uint32_t session_duration_s;// Seconds valve has been continuously flowing
    bool is_flowing;            // True if pulses are actively occurring
    uint32_t uptime_s;          // System uptime in seconds
    uint32_t free_heap;         // Free heap memory in bytes
    uint32_t free_psram;        // Free PSRAM in bytes
};
