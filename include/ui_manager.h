/**
 * @file ui_manager.h
 * @brief LVGL 8.x GUI architecture: centered droplet badge, valve state label,
 *        dynamic toggle button, real-time flow rate and volume telemetry cards.
 */

#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include "types.h"

class UiManager {
public:
    UiManager();

    /**
     * @brief Build the complete UI layout, styles, and register event callbacks
     */
    void init();

    /**
     * @brief Thread-safe update of UI elements with the latest telemetry snapshot
     * @param telemetry Current system telemetry data
     */
    void updateTelemetry(const SystemTelemetry& telemetry);

    /**
     * @brief Callback handler for the toggle button
     */
    static void onToggleBtnClicked(lv_event_t* e);

    /**
     * @brief Callback handler for clearing alarms
     */
    static void onAlarmBannerClicked(lv_event_t* e);

private:
    void createStyles();
    void buildHeader();
    void buildDropletIndicator();
    void buildValveStatus();
    void buildMetricsCards();
    void buildActionButton();
    void buildAlarmBanner();

    // LVGL Widgets
    lv_obj_t* _scr;
    lv_obj_t* _alarm_banner;
    lv_obj_t* _alarm_label;

    lv_obj_t* _droplet_container;
    lv_obj_t* _droplet_icon;

    lv_obj_t* _valve_status_label;

    lv_obj_t* _flow_rate_val_label;
    lv_obj_t* _total_vol_val_label;

    lv_obj_t* _action_btn;
    lv_obj_t* _action_btn_label;

    // Cache state to avoid unnecessary redraws
    ValveState  _last_rendered_state;
    SafetyAlarm _last_rendered_alarm;
    bool        _last_is_flowing;
    int32_t     _last_flow_rate_int;
    int32_t     _last_total_vol_int;
};

extern UiManager uiManager;
