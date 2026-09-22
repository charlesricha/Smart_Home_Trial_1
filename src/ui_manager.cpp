/**
 * @file ui_manager.cpp
 * @brief LVGL 8.x UI implementation for Smart Home Water Node
 */

#include "ui_manager.h"
#include <stdio.h>

UiManager uiManager;

// External FreeRTOS command queue defined in main.cpp
extern QueueHandle_t control_queue;

void UiManager::onToggleBtnClicked(lv_event_t* e) {
    Serial.println("[UI] Valve Action Button clicked -> dispatching CMD_VALVE_TOGGLE");
    if (control_queue != nullptr) {
        ControlCommand cmd = { ControlCommandType::CMD_VALVE_TOGGLE, 0 };
        xQueueSend(control_queue, &cmd, 0);
    }
}

void UiManager::onAlarmBannerClicked(lv_event_t* e) {
    if (control_queue != nullptr) {
        ControlCommand cmd = { ControlCommandType::CMD_RESET_ALARM, 0 };
        xQueueSend(control_queue, &cmd, 0);
    }
}

UiManager::UiManager()
    : _scr(nullptr),
      _alarm_banner(nullptr),
      _alarm_label(nullptr),
      _droplet_container(nullptr),
      _droplet_icon(nullptr),
      _valve_status_label(nullptr),
      _flow_rate_val_label(nullptr),
      _total_vol_val_label(nullptr),
      _action_btn(nullptr),
      _action_btn_label(nullptr),
      _last_rendered_state((ValveState)255),
      _last_rendered_alarm((SafetyAlarm)255),
      _last_is_flowing(false),
      _last_flow_rate_int(-1),
      _last_total_vol_int(-1) {}

void UiManager::init() {
    _scr = lv_scr_act();
    lv_obj_set_style_bg_color(_scr, lv_color_hex(0x0F172A), LV_PART_MAIN); // Slate 900
    lv_obj_clear_flag(_scr, LV_OBJ_FLAG_SCROLLABLE);

    buildHeader();
    buildDropletIndicator();
    buildValveStatus();
    buildMetricsCards();
    buildActionButton();
    buildAlarmBanner();

    Serial.println("[UI] LVGL layout and widgets created.");
}

void UiManager::buildHeader() {
    lv_obj_t* title = lv_label_create(_scr);
    lv_label_set_text(title, "AQUANODE SMART VALVE");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x94A3B8), 0); // Slate 400
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);
}

void UiManager::buildDropletIndicator() {
    // Circular Droplet Container
    _droplet_container = lv_obj_create(_scr);
    lv_obj_set_size(_droplet_container, 72, 72);
    lv_obj_align(_droplet_container, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_radius(_droplet_container, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_droplet_container, lv_color_hex(0x1E293B), 0); // Slate 800
    lv_obj_set_style_border_width(_droplet_container, 2, 0);
    lv_obj_set_style_border_color(_droplet_container, lv_color_hex(0x334155), 0);
    lv_obj_set_style_shadow_width(_droplet_container, 12, 0);
    lv_obj_set_style_shadow_color(_droplet_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(_droplet_container, LV_OPA_30, 0);
    lv_obj_clear_flag(_droplet_container, LV_OBJ_FLAG_SCROLLABLE);

    // Droplet Icon using built-in LV_SYMBOL_TINT
    _droplet_icon = lv_label_create(_droplet_container);
    lv_label_set_text(_droplet_icon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_font(_droplet_icon, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(_droplet_icon, lv_color_hex(0x64748B), 0); // Muted slate initial
    lv_obj_center(_droplet_icon);
}

void UiManager::buildValveStatus() {
    _valve_status_label = lv_label_create(_scr);
    lv_label_set_text(_valve_status_label, "VALVE: CLOSED");
    lv_obj_set_style_text_font(_valve_status_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(_valve_status_label, lv_color_hex(0xEF4444), 0); // Red-500
    lv_obj_align(_valve_status_label, LV_ALIGN_TOP_MID, 0, 110);
}

void UiManager::buildMetricsCards() {
    // Container for side-by-side cards
    // Flow Rate Card (Left)
    lv_obj_t* card_flow = lv_obj_create(_scr);
    lv_obj_set_size(card_flow, 102, 68);
    lv_obj_set_pos(card_flow, 14, 140);
    lv_obj_set_style_radius(card_flow, 10, 0);
    lv_obj_set_style_bg_color(card_flow, lv_color_hex(0x1E293B), 0);
    lv_obj_set_style_border_width(card_flow, 1, 0);
    lv_obj_set_style_border_color(card_flow, lv_color_hex(0x334155), 0);
    lv_obj_set_style_pad_all(card_flow, 6, 0);
    lv_obj_clear_flag(card_flow, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* flow_title = lv_label_create(card_flow);
    lv_label_set_text(flow_title, "FLOW RATE");
    lv_obj_set_style_text_font(flow_title, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(flow_title, lv_color_hex(0x94A3B8), 0);
    lv_obj_align(flow_title, LV_ALIGN_TOP_MID, 0, 2);

    _flow_rate_val_label = lv_label_create(card_flow);
    lv_label_set_text(_flow_rate_val_label, "0.0 L/m");
    lv_obj_set_style_text_font(_flow_rate_val_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(_flow_rate_val_label, lv_color_hex(0x38BDF8), 0); // Cyan
    lv_obj_align(_flow_rate_val_label, LV_ALIGN_BOTTOM_MID, 0, -4);

    // Total Volume Card (Right)
    lv_obj_t* card_vol = lv_obj_create(_scr);
    lv_obj_set_size(card_vol, 102, 68);
    lv_obj_set_pos(card_vol, 124, 140);
    lv_obj_set_style_radius(card_vol, 10, 0);
    lv_obj_set_style_bg_color(card_vol, lv_color_hex(0x1E293B), 0);
    lv_obj_set_style_border_width(card_vol, 1, 0);
    lv_obj_set_style_border_color(card_vol, lv_color_hex(0x334155), 0);
    lv_obj_set_style_pad_all(card_vol, 6, 0);
    lv_obj_clear_flag(card_vol, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* vol_title = lv_label_create(card_vol);
    lv_label_set_text(vol_title, "TOTAL VOL");
    lv_obj_set_style_text_font(vol_title, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(vol_title, lv_color_hex(0x94A3B8), 0);
    lv_obj_align(vol_title, LV_ALIGN_TOP_MID, 0, 2);

    _total_vol_val_label = lv_label_create(card_vol);
    lv_label_set_text(_total_vol_val_label, "0.00 L");
    lv_obj_set_style_text_font(_total_vol_val_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(_total_vol_val_label, lv_color_hex(0x34D399), 0); // Emerald
    lv_obj_align(_total_vol_val_label, LV_ALIGN_BOTTOM_MID, 0, -4);
}

void UiManager::buildActionButton() {
    _action_btn = lv_btn_create(_scr);
    lv_obj_set_size(_action_btn, 212, 50);
    lv_obj_align(_action_btn, LV_ALIGN_TOP_MID, 0, 222);
    lv_obj_set_style_radius(_action_btn, 25, 0); // Pill shape
    lv_obj_set_style_bg_color(_action_btn, lv_color_hex(0x059669), 0); // Emerald-600
    lv_obj_set_style_shadow_width(_action_btn, 14, 0);
    lv_obj_set_style_shadow_color(_action_btn, lv_color_hex(0x059669), 0);
    lv_obj_set_style_shadow_opa(_action_btn, LV_OPA_40, 0);

    lv_obj_add_event_cb(_action_btn, onToggleBtnClicked, LV_EVENT_CLICKED, nullptr);

    _action_btn_label = lv_label_create(_action_btn);
    lv_label_set_text(_action_btn_label, LV_SYMBOL_PLAY "  OPEN TAP");
    lv_obj_set_style_text_font(_action_btn_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(_action_btn_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(_action_btn_label);
}

void UiManager::buildAlarmBanner() {
    _alarm_banner = lv_obj_create(_scr);
    lv_obj_set_size(_alarm_banner, 212, 34);
    lv_obj_align(_alarm_banner, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_set_style_radius(_alarm_banner, 8, 0);
    lv_obj_set_style_bg_color(_alarm_banner, lv_color_hex(0x7F1D1D), 0); // Crimson dark
    lv_obj_set_style_border_width(_alarm_banner, 1, 0);
    lv_obj_set_style_border_color(_alarm_banner, lv_color_hex(0xEF4444), 0);
    lv_obj_set_style_pad_all(_alarm_banner, 4, 0);
    lv_obj_clear_flag(_alarm_banner, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(_alarm_banner, onAlarmBannerClicked, LV_EVENT_CLICKED, nullptr);

    _alarm_label = lv_label_create(_alarm_banner);
    lv_label_set_text(_alarm_label, LV_SYMBOL_WARNING " LEAK! TAP TO RESET");
    lv_obj_set_style_text_font(_alarm_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_alarm_label, lv_color_hex(0xFDE047), 0); // Yellow
    lv_obj_center(_alarm_label);

    // Hide banner by default
    lv_obj_add_flag(_alarm_banner, LV_OBJ_FLAG_HIDDEN);
}

void UiManager::updateTelemetry(const SystemTelemetry& telemetry) {
    char buf[32];

    // 1. Update Valve State & Action Button
    if (_last_rendered_state != telemetry.valve_state) {
        _last_rendered_state = telemetry.valve_state;

        switch (telemetry.valve_state) {
            case ValveState::OPEN:
                lv_label_set_text(_valve_status_label, "VALVE: OPEN");
                lv_obj_set_style_text_color(_valve_status_label, lv_color_hex(0x10B981), 0); // Emerald

                lv_label_set_text(_action_btn_label, LV_SYMBOL_STOP "  CLOSE TAP");
                lv_obj_set_style_bg_color(_action_btn, lv_color_hex(0xDC2626), 0); // Crimson Red
                lv_obj_set_style_shadow_color(_action_btn, lv_color_hex(0xDC2626), 0);
                break;

            case ValveState::CLOSED:
                lv_label_set_text(_valve_status_label, "VALVE: CLOSED");
                lv_obj_set_style_text_color(_valve_status_label, lv_color_hex(0xEF4444), 0); // Red

                lv_label_set_text(_action_btn_label, LV_SYMBOL_PLAY "  OPEN TAP");
                lv_obj_set_style_bg_color(_action_btn, lv_color_hex(0x059669), 0); // Emerald
                lv_obj_set_style_shadow_color(_action_btn, lv_color_hex(0x059669), 0);
                break;

            case ValveState::MOVING_TO_OPEN:
                lv_label_set_text(_valve_status_label, "VALVE: OPENING...");
                lv_obj_set_style_text_color(_valve_status_label, lv_color_hex(0xF59E0B), 0); // Amber
                break;

            case ValveState::MOVING_TO_CLOSE:
                lv_label_set_text(_valve_status_label, "VALVE: CLOSING...");
                lv_obj_set_style_text_color(_valve_status_label, lv_color_hex(0xF59E0B), 0); // Amber
                break;
        }
    }

    // 2. Update Water Droplet Glow/Color based on active flow
    if (_last_is_flowing != telemetry.is_flowing || _last_rendered_state != telemetry.valve_state) {
        _last_is_flowing = telemetry.is_flowing;
        if (telemetry.is_flowing) {
            // Vibrant glowing water droplet
            lv_obj_set_style_text_color(_droplet_icon, lv_color_hex(0x38BDF8), 0); // Vivid Sky Blue
            lv_obj_set_style_border_color(_droplet_container, lv_color_hex(0x0284C7), 0);
            lv_obj_set_style_shadow_color(_droplet_container, lv_color_hex(0x0284C7), 0);
            lv_obj_set_style_shadow_opa(_droplet_container, LV_OPA_70, 0);
        } else {
            // Idle droplet
            lv_obj_set_style_text_color(_droplet_icon,
                (telemetry.valve_state == ValveState::OPEN) ? lv_color_hex(0x0284C7) : lv_color_hex(0x475569), 0);
            lv_obj_set_style_border_color(_droplet_container, lv_color_hex(0x334155), 0);
            lv_obj_set_style_shadow_opa(_droplet_container, LV_OPA_20, 0);
        }
    }

    // 3. Update Flow Rate Readout (in tenths of L/min to prevent rapid label jitter)
    int32_t current_flow_int = (int32_t)(telemetry.flow_rate_lpm * 10.0f);
    if (_last_flow_rate_int != current_flow_int) {
        _last_flow_rate_int = current_flow_int;
        snprintf(buf, sizeof(buf), "%.1f L/m", telemetry.flow_rate_lpm);
        lv_label_set_text(_flow_rate_val_label, buf);
    }

    // 4. Update Total Volume Readout (in hundredths of Liter)
    int32_t current_vol_int = (int32_t)(telemetry.total_volume_l * 100.0f);
    if (_last_total_vol_int != current_vol_int) {
        _last_total_vol_int = current_vol_int;
        snprintf(buf, sizeof(buf), "%.2f L", telemetry.total_volume_l);
        lv_label_set_text(_total_vol_val_label, buf);
    }

    // 5. Update Safety Alarm Banner
    if (_last_rendered_alarm != telemetry.safety_alarm) {
        _last_rendered_alarm = telemetry.safety_alarm;

        if (telemetry.safety_alarm == SafetyAlarm::NONE) {
            lv_obj_add_flag(_alarm_banner, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(_alarm_banner, LV_OBJ_FLAG_HIDDEN);
            if (telemetry.safety_alarm == SafetyAlarm::LEAK_DETECTED) {
                lv_label_set_text(_alarm_label, LV_SYMBOL_WARNING " LEAK DETECTED! RESET");
            } else if (telemetry.safety_alarm == SafetyAlarm::MAX_TIME_EXCEEDED) {
                lv_label_set_text(_alarm_label, LV_SYMBOL_WARNING " TIMEOUT SHUTOFF! RESET");
            } else if (telemetry.safety_alarm == SafetyAlarm::MAX_VOLUME_EXCEEDED) {
                lv_label_set_text(_alarm_label, LV_SYMBOL_WARNING " OVER-VOL SHUTOFF! RESET");
            }
        }
    }
}
