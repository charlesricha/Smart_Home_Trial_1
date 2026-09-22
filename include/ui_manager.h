/**
 * @file ui_manager.h
 * @brief Smart Home Dashboard UI Architecture (Landscape 320x240)
 *        - Block 1 (Top-Left): Music Player with Active Speaker Badge
 *        - Block 2 (Bottom-Left): Large Clock, Date, Temperature & Humidity
 *        - Block 3 (Right Column): Interactive Rooms Menu
 */

#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include "types.h"

struct MusicTrack {
    const char* title;
    const char* artist;
    uint16_t duration_sec;
};

class UiManager {
public:
    UiManager();

    void init();
    void updateTelemetry(const SystemTelemetry& telemetry);

    // LVGL Event callbacks
    static void onRoomBtnClicked(lv_event_t* e);
    static void onMusicPlayPauseClicked(lv_event_t* e);
    static void onMusicPrevClicked(lv_event_t* e);
    static void onMusicNextClicked(lv_event_t* e);

    void selectRoom(uint8_t index);
    void updateMusicTrack(uint8_t index);

private:
    void buildMusicBlock();
    void buildClimateBlock();
    void buildRoomsBlock();

    lv_obj_t* _scr;

    // Block 1: Music Player
    lv_obj_t* _music_card;
    lv_obj_t* _speaker_pill;
    lv_obj_t* _speaker_label;
    lv_obj_t* _music_status_label;
    lv_obj_t* _track_title_label;
    lv_obj_t* _artist_label;
    lv_obj_t* _btn_prev;
    lv_obj_t* _btn_play;
    lv_obj_t* _btn_play_label;
    lv_obj_t* _btn_next;
    lv_obj_t* _music_bar;
    lv_obj_t* _time_pos_label;

    // Block 2: Time & Climate
    lv_obj_t* _climate_card;
    lv_obj_t* _clock_label;
    lv_obj_t* _ampm_label;
    lv_obj_t* _date_label;
    lv_obj_t* _temp_val_label;
    lv_obj_t* _humid_val_label;

    // Block 3: Rooms Menu
    lv_obj_t* _rooms_card;
    lv_obj_t* _room_btns[5];
    lv_obj_t* _room_labels[5];

    // State
    uint8_t _selected_room_idx;
    uint8_t _current_track_idx;
    bool _is_playing;
    uint32_t _track_elapsed_sec;
    uint32_t _last_clock_tick_ms;
    uint32_t _last_progress_tick_ms;

    // Clock state
    uint8_t _clock_hours;
    uint8_t _clock_mins;
    uint8_t _clock_secs;
};

extern UiManager uiManager;
