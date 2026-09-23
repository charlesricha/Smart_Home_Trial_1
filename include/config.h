/**
 * @file config.h
 * @brief System Pinout, Hardware Configuration & Safety Parameters
 * Board: VIEWE UEDX24320028E-WB-A (ESP32-S3-WROOM-1-N16R8)
 */

#pragma once
#include <Arduino.h>

// =============================================================================
// HARDWARE PIN DEFINITIONS
// =============================================================================

/**
 * @warning CRITICAL HARDWARE VERIFICATION NOTE:
 * On some VIEWE UEDX24320028E board revisions, GPIO 13 is internally routed to
 * the LCD Backlight (BL) LED driver transistor.
 * If your board uses GPIO 13 for Backlight:
 *   1. Using GPIO 13 for the flow sensor will cause bus contention or screen flickering.
 *   2. Reassign PIN_FLOW_SENSOR to another available breakout GPIO (e.g., IO10, IO11, IO12).
 * If your board exposes GPIO 13 on the J1/J2 header and BL is tied to 3.3V or another pin,
 * GPIO 13 is safe to use as defined below.
 */
#define PIN_FLOW_SENSOR         10  // Reassigned from 13 to avoid backlight conflict
#define PIN_SERVO_PWM           14  // SG90 Micro Servo PWM control line

// VIEWE UEDX24320028E Display (GC9307 SPI)
#define PIN_LCD_CS              42
#define PIN_LCD_SCLK            40
#define PIN_LCD_MOSI            45
#define PIN_LCD_DC              41
#define PIN_LCD_RST             39
#define PIN_LCD_BL              13  // LCD Backlight LED enable / PWM (active HIGH)
#define PIN_LCD_IM0             47  // GC9307 mode select IM0 -> LOW for 4-wire SPI
#define PIN_LCD_IM1             48  // GC9307 mode select IM1 -> HIGH for 4-wire SPI

// VIEWE UEDX24320028E Capacitive Touch (CHSC6540 I2C)
#define PIN_TOUCH_SDA           1
#define PIN_TOUCH_SCL           3
#define PIN_TOUCH_INT           4
#define PIN_TOUCH_RST           2

// Screen Dimensions (Landscape)
#define LCD_WIDTH               320
#define LCD_HEIGHT              240

// =============================================================================
// FLOW SENSOR CALIBRATION & SAMPLING
// =============================================================================
// Standard YF-S201: F (Hz) = 7.5 * Q (L/min) -> 450 pulses per Liter
#define FLOW_CALIBRATION_FACTOR 7.5f     // Pulses/sec per L/min
#define PULSES_PER_LITER        450.0f   // Total pulses in one liter of water
#define MIN_PULSE_INTERVAL_US   100      // 100us debounce (supports fast pulse trains & air testing up to 10kHz)
#define FLOW_SAMPLE_PERIOD_MS   500      // Sampling and calculation window (500 ms)

// NVS Persistence Wear-Leveling
#define NVS_NAMESPACE           "aquanode"
#define NVS_KEY_TOTAL_VOL       "tot_vol"
#define NVS_VOL_COMMIT_DELTA_L  1.0f     // Commit every 1.0 Liter change
#define NVS_IDLE_COMMIT_DELAY_MS 3000    // Commit 3 seconds after flow stops

// =============================================================================
// SERVO ACTUATOR PARAMETERS (SG90)
// =============================================================================
#define SERVO_LEDC_CHANNEL      0
#define SERVO_LEDC_FREQ         50       // 50Hz standard servo frequency
#define SERVO_LEDC_RES_BITS     14       // 14-bit resolution: 0 - 16383 (1 count = 1.22 us)
#define SERVO_MIN_PULSE_US      500      // 500 us = 0 degrees (CLOSED)
#define SERVO_MAX_PULSE_US      2400     // 2400 us = 90/180 degrees (OPEN)
#define SERVO_ANGLE_CLOSED      0        // Valve CLOSED angle
#define SERVO_ANGLE_OPEN        90       // Valve OPEN angle (Quarter-turn ball valve)

// Detach timeout: Shuts off PWM after motion completes to prevent SG90 jitter,
// buzzing noise, thermal runaway, and 5V rail brownouts.
#define SERVO_DETACH_DELAY_MS   700

// =============================================================================
// SAFETY SUPERVISOR & FLOOD PROTECTION
// =============================================================================
#define SAFETY_MAX_CONTINUOUS_SEC 900    // 15 minutes continuous flow cutoff
#define SAFETY_MAX_SESSION_VOL_L  50.0f  // 50 Liters single session cutoff
#define SAFETY_LEAK_THRESH_LPM    0.20f  // Trigger leak alarm if flow > 0.20 L/min when CLOSED

// =============================================================================
// NETWORK & mDNS CONFIGURATION (MILESTONE 1)
// =============================================================================
// Set your local WiFi credentials here:
#define DEFAULT_WIFI_SSID       "raspberry"
#define DEFAULT_WIFI_PASS       "Ashnephy25"
#define WIFI_CONNECT_TIMEOUT_MS 10000

// Device Hostname for local mDNS (http://smarthome-kitchen.local)
#define DEVICE_HOSTNAME         "smarthome-kitchen"
#define DEVICE_FRIENDLY_NAME    "Kitchen Water Controller"
#define DEVICE_ROOM             "Kitchen"

// SoftAP fallback if WiFi is not connected
#define AP_SSID_PREFIX          "SmartHome-Node-"
#define AP_DEFAULT_PASS         "12345678"

// =============================================================================
// FREERTOS TASK ALLOCATION (ESP32-S3 DUAL CORE)
// =============================================================================
#define CORE_UI                 1        // Core 1: LVGL rendering & touch handling
#define CORE_CONTROL            0        // Core 0: Flow sensor ISR processing, servo, safety supervisor
#define CORE_NETWORK            0        // Core 0: WiFi, mDNS, REST API webserver

#define UI_TASK_STACK_SIZE      8192
#define UI_TASK_PRIORITY        2

#define CONTROL_TASK_STACK_SIZE 4096
#define CONTROL_TASK_PRIORITY   3

#define NETWORK_TASK_STACK_SIZE 6144
#define NETWORK_TASK_PRIORITY   1

#define UI_REFRESH_PERIOD_MS    5        // 5ms LVGL timer loop

