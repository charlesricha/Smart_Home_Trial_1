/**
 * @file valve_controller.h
 * @brief Servo valve actuator driver with hardware LEDC PWM and auto-detach protection
 */

#pragma once

#include <Arduino.h>
#include "config.h"
#include "types.h"

class ValveController {
public:
    ValveController();

    /**
     * @brief Initialize LEDC peripheral and ensure valve is in safe CLOSED position
     */
    void begin();

    /**
     * @brief Command the valve to OPEN (90 degrees)
     */
    void open();

    /**
     * @brief Command the valve to CLOSE (0 degrees)
     */
    void close();

    /**
     * @brief Toggle the valve between OPEN and CLOSED
     */
    void toggle();

    /**
     * @brief Periodic update loop called in control task to handle auto-detach timeout
     * @param now_ms Current millis() timestamp
     */
    void update(uint32_t now_ms);

    /**
     * @brief Get the current operational state of the valve
     */
    ValveState getState() const { return _state; }

    /**
     * @brief Check if valve is fully open
     */
    bool isOpen() const { return _state == ValveState::OPEN; }

    /**
     * @brief Check if valve is currently in motion
     */
    bool isMoving() const {
        return _state == ValveState::MOVING_TO_OPEN || _state == ValveState::MOVING_TO_CLOSE;
    }

private:
    void writeAngle(uint32_t angle);
    void detachPwm();

    ValveState _state;
    uint32_t   _current_angle;
    uint32_t   _motion_start_ms;
    bool       _pwm_attached;
};

extern ValveController valveController;
