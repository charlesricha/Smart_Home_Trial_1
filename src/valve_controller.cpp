/**
 * @file valve_controller.cpp
 * @brief Implementation of servo actuator control with LEDC and anti-jitter auto-detach
 */

#include "valve_controller.h"
#include <esp_arduino_version.h>

ValveController valveController;

ValveController::ValveController()
    : _state(ValveState::CLOSED),
      _current_angle(SERVO_ANGLE_CLOSED),
      _motion_start_ms(0),
      _pwm_attached(false) {}

void ValveController::begin() {
    Serial.printf("[VALVE] Initializing SG90 servo PWM on GPIO %d (50Hz, 14-bit resolution)...\n", PIN_SERVO_PWM);

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcAttach(PIN_SERVO_PWM, SERVO_LEDC_FREQ, SERVO_LEDC_RES_BITS);
#else
    ledcSetup(SERVO_LEDC_CHANNEL, SERVO_LEDC_FREQ, SERVO_LEDC_RES_BITS);
    ledcAttachPin(PIN_SERVO_PWM, SERVO_LEDC_CHANNEL);
#endif
    _pwm_attached = true;

    // Command default safe closed state on startup
    close();
}

void ValveController::writeAngle(uint32_t angle) {
    if (angle > 180) angle = 180;
    _current_angle = angle;

    // Calculate duty cycle: 500us (0 deg) to 2400us (180 deg)
    float pulse_us = (float)SERVO_MIN_PULSE_US +
                     ((float)angle / 180.0f) * (float)(SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US);

    uint32_t duty = (uint32_t)((pulse_us / 20000.0f) * 16383.0f);

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    if (!_pwm_attached) {
        ledcAttach(PIN_SERVO_PWM, SERVO_LEDC_FREQ, SERVO_LEDC_RES_BITS);
        _pwm_attached = true;
    }
    ledcWrite(PIN_SERVO_PWM, duty);
#else
    if (!_pwm_attached) {
        ledcAttachPin(PIN_SERVO_PWM, SERVO_LEDC_CHANNEL);
        _pwm_attached = true;
    }
    ledcWrite(SERVO_LEDC_CHANNEL, duty);
#endif

    _motion_start_ms = millis();
}

void ValveController::detachPwm() {
    if (_pwm_attached) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
        ledcWrite(PIN_SERVO_PWM, 0);
        ledcDetach(PIN_SERVO_PWM);
#else
        ledcWrite(SERVO_LEDC_CHANNEL, 0);
        ledcDetachPin(PIN_SERVO_PWM);
#endif
        pinMode(PIN_SERVO_PWM, OUTPUT);
        digitalWrite(PIN_SERVO_PWM, LOW);
        _pwm_attached = false;
        Serial.println("[VALVE] Motion finished -> PWM detached. Actuator quiet, 5V rail protected.");
    }
}

void ValveController::open() {
    Serial.println("[VALVE] Command -> OPEN");
    _state = ValveState::MOVING_TO_OPEN;
    writeAngle(SERVO_ANGLE_OPEN);
}

void ValveController::close() {
    Serial.println("[VALVE] Command -> CLOSE");
    _state = ValveState::MOVING_TO_CLOSE;
    writeAngle(SERVO_ANGLE_CLOSED);
}

void ValveController::toggle() {
    if (_state == ValveState::OPEN || _state == ValveState::MOVING_TO_OPEN) {
        close();
    } else {
        open();
    }
}

void ValveController::update(uint32_t now_ms) {
    // If servo is currently moving, check if transit delay has elapsed
    if (isMoving() && _pwm_attached) {
        if (now_ms - _motion_start_ms >= SERVO_DETACH_DELAY_MS) {
            detachPwm();
            if (_state == ValveState::MOVING_TO_OPEN) {
                _state = ValveState::OPEN;
                Serial.println("[VALVE] State transition: OPEN");
            } else if (_state == ValveState::MOVING_TO_CLOSE) {
                _state = ValveState::CLOSED;
                Serial.println("[VALVE] State transition: CLOSED");
            }
        }
    }
}
