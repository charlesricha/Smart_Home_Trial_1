
/**
 * @file main.cpp
 * @brief Smart Home Water Control Node Firmware
 * Board: VIEWE UEDX24320028E-WB-A (ESP32-S3-WROOM-1-N16R8)
 *
 * FreeRTOS Dual-Core Architecture:
 * - Core 1: LVGL 8.x UI Rendering & Capacitive Touch Polling
 * - Core 0: Flow Sensor ISR Processing, SG90 Actuation & Safety Supervision
 */

#include <Arduino.h>
#include "config.h"
#include "types.h"
#include "display_driver.h"
#include "flow_sensor.h"
#include "valve_controller.h"
#include "safety_supervisor.h"
#include "ui_manager.h"

// FreeRTOS Synchronization Primitives
QueueHandle_t control_queue = nullptr;
static SemaphoreHandle_t telemetry_mutex = nullptr;
static SystemTelemetry shared_telemetry;

// Task Handles
static TaskHandle_t controlTaskHandle = nullptr;
static TaskHandle_t uiTaskHandle = nullptr;

// =============================================================================
// CONTROL TASK (CORE 0): Real-Time Sensors, Servo & Safety Supervisor
// =============================================================================
void control_task_func(void* pvParameters) {
    Serial.printf("[SYSTEM] Control Task running on Core %d (Priority %d)\n",
                  xPortGetCoreID(), uxTaskPriorityGet(NULL));

    uint32_t last_log_ms = 0;

    for (;;) {
        uint32_t now_ms = millis();

        // 1. Process incoming control commands from UI or external sources
        ControlCommand cmd;
        while (xQueueReceive(control_queue, &cmd, 0) == pdTRUE) {
            switch (cmd.type) {
                case ControlCommandType::CMD_VALVE_TOGGLE:
                    Serial.println("[CMD] Valve toggle requested");
                    valveController.toggle();
                    if (valveController.isOpen()) {
                        flowSensor.resetSession();
                    }
                    break;

                case ControlCommandType::CMD_VALVE_OPEN:
                    Serial.println("[CMD] Valve open requested");
                    valveController.open();
                    flowSensor.resetSession();
                    break;

                case ControlCommandType::CMD_VALVE_CLOSE:
                    Serial.println("[CMD] Valve close requested");
                    valveController.close();
                    break;

                case ControlCommandType::CMD_RESET_ALARM:
                    Serial.println("[CMD] Alarm reset requested");
                    safetySupervisor.resetAlarm();
                    break;

                case ControlCommandType::CMD_RESET_VOLUME:
                    Serial.println("[CMD] Volume reset requested");
                    flowSensor.resetTotal();
                    break;
            }
        }

        // 2. Update Actuator State Machine (LEDC PWM & Auto-Detach timer)
        valveController.update(now_ms);

        // 3. Update Flow Sensor (Frequency, Flow Rate, Volume Integration & NVS)
        flowSensor.update(now_ms);

        // 4. Update Safety Supervisor (Timeout, Over-Volume, Leak Detection)
        safetySupervisor.update(now_ms);

        // 5. Update Shared Telemetry under Mutex for UI Task
        if (xSemaphoreTake(telemetry_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            shared_telemetry.valve_state = valveController.getState();
            shared_telemetry.safety_alarm = safetySupervisor.getActiveAlarm();
            shared_telemetry.flow_rate_lpm = flowSensor.getInstantFlowRate();
            shared_telemetry.session_volume_l = flowSensor.getSessionVolume();
            shared_telemetry.total_volume_l = flowSensor.getTotalVolume();
            shared_telemetry.session_duration_s = flowSensor.getSessionDurationSec();
            shared_telemetry.is_flowing = flowSensor.isFlowing();
            shared_telemetry.uptime_s = now_ms / 1000;
            shared_telemetry.free_heap = esp_get_free_heap_size();
            shared_telemetry.free_psram = ESP.getFreePsram();
            xSemaphoreGive(telemetry_mutex);
        }

        // 6. Periodic Serial Diagnostics (every 2000 ms)
        if (now_ms - last_log_ms >= 2000) {
            last_log_ms = now_ms;
            Serial.printf("[DIAG] Valve: %s | Flow: %5.2f L/min | Tot: %6.2f L | Alarm: %d | Heap: %u\n",
                          valveController.isOpen() ? "OPEN " : "CLOSE",
                          flowSensor.getInstantFlowRate(),
                          flowSensor.getTotalVolume(),
                          (int)safetySupervisor.getActiveAlarm(),
                          esp_get_free_heap_size());
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // 50 Hz control loop
    }
}

// =============================================================================
// UI TASK (CORE 1): LVGL 8.x Engine & Touch Input Handling
// =============================================================================
void ui_task_func(void* pvParameters) {
    Serial.printf("[SYSTEM] UI Task running on Core %d (Priority %d)\n",
                  xPortGetCoreID(), uxTaskPriorityGet(NULL));

    uint32_t last_ui_update_ms = 0;

    for (;;) {
        uint32_t now_ms = millis();

        // 1. Periodically fetch telemetry snapshot and update LVGL widgets (every 50ms)
        if (now_ms - last_ui_update_ms >= 50) {
            last_ui_update_ms = now_ms;
            SystemTelemetry local_copy;
            if (xSemaphoreTake(telemetry_mutex, 0) == pdTRUE) {
                local_copy = shared_telemetry;
                xSemaphoreGive(telemetry_mutex);
                uiManager.updateTelemetry(local_copy);
            }
        }

        // 2. LVGL Timer & Screen Refresh Loop
        lv_timer_handler();

        // 5ms delay yields CPU and maintains smooth 60fps rendering
        vTaskDelay(pdMS_TO_TICKS(UI_REFRESH_PERIOD_MS));
    }
}

// =============================================================================
// SETUP & INITIALIZATION
// =============================================================================
void setup() {
    Serial.begin(115200);
    delay(500); // Allow USB CDC connection to stabilize

    Serial.println("\n========================================================");
    Serial.println("  Smart Home Water Control Node");
    Serial.println("  Board: VIEWE UEDX24320028E-WB-A (ESP32-S3 N16R8)");
    Serial.println("  Firmware: Dual-Core FreeRTOS + LVGL 8.3 + LovyanGFX");
    Serial.println("========================================================\n");

    // Create FreeRTOS synchronization primitives
    control_queue = xQueueCreate(10, sizeof(ControlCommand));
    telemetry_mutex = xSemaphoreCreateMutex();

    if (!control_queue || !telemetry_mutex) {
        Serial.println("[FATAL] Failed to create FreeRTOS sync primitives!");
        while (1) { delay(1000); }
    }

    // Initialize Subsystems
    valveController.begin();
    flowSensor.begin();
    safetySupervisor.begin();

    // Initialize Display and Touch HAL
    if (!display_driver_init()) {
        Serial.println("[FATAL] Display initialization failed!");
        while (1) { delay(1000); }
    }

    // Initialize LVGL UI Layout
    uiManager.init();

    // Spawn FreeRTOS Tasks pinned to respective cores
    xTaskCreatePinnedToCore(
        control_task_func,
        "ControlTask",
        CONTROL_TASK_STACK_SIZE,
        NULL,
        CONTROL_TASK_PRIORITY,
        &controlTaskHandle,
        CORE_CONTROL // Core 0
    );

    xTaskCreatePinnedToCore(
        ui_task_func,
        "UiTask",
        UI_TASK_STACK_SIZE,
        NULL,
        UI_TASK_PRIORITY,
        &uiTaskHandle,
        CORE_UI // Core 1
    );

    Serial.println("[SYSTEM] All tasks spawned. System operational.");
}

void loop() {
    while (Serial.available()) {
        char ch = Serial.read();
        ControlCommand cmd;
        switch (ch) {
            case 'o':
            case 'O':
                cmd.type = ControlCommandType::CMD_VALVE_OPEN;
                xQueueSend(control_queue, &cmd, 0);
                break;
            case 'c':
            case 'C':
                cmd.type = ControlCommandType::CMD_VALVE_CLOSE;
                xQueueSend(control_queue, &cmd, 0);
                break;
            case 't':
            case 'T':
                cmd.type = ControlCommandType::CMD_VALVE_TOGGLE;
                xQueueSend(control_queue, &cmd, 0);
                break;
            case 'r':
            case 'R':
                cmd.type = ControlCommandType::CMD_RESET_ALARM;
                xQueueSend(control_queue, &cmd, 0);
                break;
            case 'v':
            case 'V':
                cmd.type = ControlCommandType::CMD_RESET_VOLUME;
                xQueueSend(control_queue, &cmd, 0);
                break;
            case '?':
                Serial.println("\n--- Interactive Water Control Console ---");
                Serial.printf("Flow Sensor Pin: GPIO %d | Servo Pin: GPIO %d\n", PIN_FLOW_SENSOR, PIN_SERVO_PWM);
                Serial.println("Commands: [o]pen valve, [c]lose valve, [t]oggle valve, [r]eset alarm, [v]olume reset");
                break;
        }
    }
    vTaskDelay(pdMS_TO_TICKS(50));
}
