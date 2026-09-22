/**
 * @file ui_manager.cpp
 * @brief Smart Home Dashboard UI Implementation (320x240 Landscape)
 *        - Block 1: Music Player & Speaker Badge (~70% width, top)
 *        - Block 2: Large Clock, Date, Temp & Humidity (~70% width, bottom)
 *        - Block 3: Rooms Menu (30% width, right column)
 */

#include "ui_manager.h"
#include <stdio.h>

UiManager uiManager;

static const char* ROOM_NAMES[5] = {
    "Living Room",
    "Kitchen",
    "Dining",
    "Bedroom",
    "Bathroom"
};

static const MusicTrack PLAYLIST[4] = {
    { "Blinding Lights", "The Weeknd", 200 },
    { "As It Was", "Harry Styles", 167 },
    { "Levitating", "Dua Lipa", 203 },
    { "Shape of You", "Ed Sheeran", 233 }
};
static const uint8_t NUM_TRACKS = sizeof(PLAYLIST) / sizeof(PLAYLIST[0]);

void UiManager::onRoomBtnClicked(lv_event_t* e) {
    uint8_t idx = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    uiManager.selectRoom(idx);
}

void UiManager::onMusicPlayPauseClicked(lv_event_t* e) {
    (void)e;
    uiManager._is_playing = !uiManager._is_playing;
    if (uiManager._is_playing) {
        lv_label_set_text(uiManager._btn_play_label, LV_SYMBOL_PAUSE);
        lv_obj_set_style_bg_color(uiManager._btn_play, lv_color_hex(0x2563EB), 0);
        lv_label_set_text(uiManager._music_status_label, LV_SYMBOL_AUDIO " Playing");
        lv_obj_set_style_text_color(uiManager._music_status_label, lv_color_hex(0x10B981), 0);
        Serial.println("[UI] Music: PLAY");
    } else {
        lv_label_set_text(uiManager._btn_play_label, LV_SYMBOL_PLAY);
        lv_obj_set_style_bg_color(uiManager._btn_play, lv_color_hex(0x475569), 0);
        lv_label_set_text(uiManager._music_status_label, LV_SYMBOL_PAUSE " Paused");
        lv_obj_set_style_text_color(uiManager._music_status_label, lv_color_hex(0x94A3B8), 0);
        Serial.println("[UI] Music: PAUSE");
    }
}

void UiManager::onMusicNextClicked(lv_event_t* e) {
    (void)e;
    uiManager._current_track_idx = (uiManager._current_track_idx + 1) % NUM_TRACKS;
    uiManager.updateMusicTrack(uiManager._current_track_idx);
    Serial.printf("[UI] Music Next: %s\n", PLAYLIST[uiManager._current_track_idx].title);
}

void UiManager::onMusicPrevClicked(lv_event_t* e) {
    (void)e;
    uiManager._current_track_idx = (uiManager._current_track_idx == 0)
                                    ? (NUM_TRACKS - 1)
                                    : (uiManager._current_track_idx - 1);
    uiManager.updateMusicTrack(uiManager._current_track_idx);
    Serial.printf("[UI] Music Prev: %s\n", PLAYLIST[uiManager._current_track_idx].title);
}

UiManager::UiManager()
    : _scr(nullptr),
      _music_card(nullptr),
      _speaker_pill(nullptr),
      _speaker_label(nullptr),
      _music_status_label(nullptr),
      _track_title_label(nullptr),
      _artist_label(nullptr),
      _btn_prev(nullptr),
      _btn_play(nullptr),
      _btn_play_label(nullptr),
      _btn_next(nullptr),
      _music_bar(nullptr),
      _time_pos_label(nullptr),
      _climate_card(nullptr),
      _clock_label(nullptr),
      _ampm_label(nullptr),
      _date_label(nullptr),
      _temp_val_label(nullptr),
      _humid_val_label(nullptr),
      _rooms_card(nullptr),
      _selected_room_idx(0),
      _current_track_idx(0),
      _is_playing(true),
      _track_elapsed_sec(45),
      _last_clock_tick_ms(0),
      _last_progress_tick_ms(0),
      _clock_hours(10),
      _clock_mins(45),
      _clock_secs(0) {
    for (int i = 0; i < 5; i++) {
        _room_btns[i] = nullptr;
        _room_labels[i] = nullptr;
    }
}

void UiManager::init() {
    _scr = lv_scr_act();
    lv_obj_set_style_bg_color(_scr, lv_color_hex(0x0B0F19), LV_PART_MAIN); // Rich dark slate background
    lv_obj_clear_flag(_scr, LV_OBJ_FLAG_SCROLLABLE);

    buildMusicBlock();
    buildClimateBlock();
    buildRoomsBlock();

    // Default room selection
    selectRoom(0);

    Serial.println("[UI] Smart Home Landscape Dashboard initialized.");
}

void UiManager::buildMusicBlock() {
    // Container: X=4, Y=4, W=214, H=104
    _music_card = lv_obj_create(_scr);
    lv_obj_set_pos(_music_card, 4, 4);
    lv_obj_set_size(_music_card, 214, 104);
    lv_obj_set_style_bg_color(_music_card, lv_color_hex(0x141C2B), 0);
    lv_obj_set_style_border_color(_music_card, lv_color_hex(0x232F42), 0);
    lv_obj_set_style_border_width(_music_card, 1, 0);
    lv_obj_set_style_radius(_music_card, 10, 0);
    lv_obj_set_style_pad_all(_music_card, 0, 0);
    lv_obj_clear_flag(_music_card, LV_OBJ_FLAG_SCROLLABLE);

    // Active Speaker Pill Badge
    _speaker_pill = lv_obj_create(_music_card);
    lv_obj_set_pos(_speaker_pill, 6, 6);
    lv_obj_set_size(_speaker_pill, LV_SIZE_CONTENT, 20);
    lv_obj_set_style_radius(_speaker_pill, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_speaker_pill, lv_color_hex(0x1E293B), 0);
    lv_obj_set_style_border_color(_speaker_pill, lv_color_hex(0x38BDF8), 0);
    lv_obj_set_style_border_width(_speaker_pill, 1, 0);
    lv_obj_set_style_pad_hor(_speaker_pill, 8, 0);
    lv_obj_set_style_pad_ver(_speaker_pill, 1, 0);
    lv_obj_clear_flag(_speaker_pill, LV_OBJ_FLAG_SCROLLABLE);

    _speaker_label = lv_label_create(_speaker_pill);
    lv_label_set_text_fmt(_speaker_label, "%s Living Room", LV_SYMBOL_VOLUME_MAX);
    lv_obj_set_style_text_font(_speaker_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_speaker_label, lv_color_hex(0x38BDF8), 0);
    lv_obj_center(_speaker_label);

    // Status Indicator
    _music_status_label = lv_label_create(_music_card);
    lv_label_set_text(_music_status_label, LV_SYMBOL_AUDIO " Playing");
    lv_obj_set_style_text_font(_music_status_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_music_status_label, lv_color_hex(0x10B981), 0);
    lv_obj_align(_music_status_label, LV_ALIGN_TOP_RIGHT, -8, 8);

    // Track Title
    _track_title_label = lv_label_create(_music_card);
    lv_label_set_text(_track_title_label, PLAYLIST[0].title);
    lv_obj_set_style_text_font(_track_title_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_track_title_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(_track_title_label, 8, 30);

    // Artist Label
    _artist_label = lv_label_create(_music_card);
    lv_label_set_text(_artist_label, PLAYLIST[0].artist);
    lv_obj_set_style_text_font(_artist_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_artist_label, lv_color_hex(0x94A3B8), 0);
    lv_obj_set_pos(_artist_label, 8, 48);

    // Prev Button
    _btn_prev = lv_btn_create(_music_card);
    lv_obj_set_pos(_btn_prev, 8, 68);
    lv_obj_set_size(_btn_prev, 26, 24);
    lv_obj_set_style_bg_color(_btn_prev, lv_color_hex(0x1E293B), 0);
    lv_obj_set_style_border_color(_btn_prev, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_width(_btn_prev, 1, 0);
    lv_obj_set_style_radius(_btn_prev, 6, 0);
    lv_obj_set_style_pad_all(_btn_prev, 0, 0);
    lv_obj_t* prev_lbl = lv_label_create(_btn_prev);
    lv_label_set_text(prev_lbl, LV_SYMBOL_PREV);
    lv_obj_set_style_text_font(prev_lbl, &lv_font_montserrat_12, 0);
    lv_obj_center(prev_lbl);
    lv_obj_add_event_cb(_btn_prev, onMusicPrevClicked, LV_EVENT_CLICKED, NULL);

    // Play/Pause Button
    _btn_play = lv_btn_create(_music_card);
    lv_obj_set_pos(_btn_play, 38, 66);
    lv_obj_set_size(_btn_play, 30, 26);
    lv_obj_set_style_bg_color(_btn_play, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_border_color(_btn_play, lv_color_hex(0x3B82F6), 0);
    lv_obj_set_style_border_width(_btn_play, 1, 0);
    lv_obj_set_style_radius(_btn_play, 6, 0);
    lv_obj_set_style_pad_all(_btn_play, 0, 0);
    _btn_play_label = lv_label_create(_btn_play);
    lv_label_set_text(_btn_play_label, LV_SYMBOL_PAUSE);
    lv_obj_set_style_text_font(_btn_play_label, &lv_font_montserrat_14, 0);
    lv_obj_center(_btn_play_label);
    lv_obj_add_event_cb(_btn_play, onMusicPlayPauseClicked, LV_EVENT_CLICKED, NULL);

    // Next Button
    _btn_next = lv_btn_create(_music_card);
    lv_obj_set_pos(_btn_next, 72, 68);
    lv_obj_set_size(_btn_next, 26, 24);
    lv_obj_set_style_bg_color(_btn_next, lv_color_hex(0x1E293B), 0);
    lv_obj_set_style_border_color(_btn_next, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_width(_btn_next, 1, 0);
    lv_obj_set_style_radius(_btn_next, 6, 0);
    lv_obj_set_style_pad_all(_btn_next, 0, 0);
    lv_obj_t* next_lbl = lv_label_create(_btn_next);
    lv_label_set_text(next_lbl, LV_SYMBOL_NEXT);
    lv_obj_set_style_text_font(next_lbl, &lv_font_montserrat_12, 0);
    lv_obj_center(next_lbl);
    lv_obj_add_event_cb(_btn_next, onMusicNextClicked, LV_EVENT_CLICKED, NULL);

    // Mini Progress Bar
    _music_bar = lv_bar_create(_music_card);
    lv_obj_set_pos(_music_bar, 104, 78);
    lv_obj_set_size(_music_bar, 54, 4);
    lv_bar_set_range(_music_bar, 0, 100);
    lv_bar_set_value(_music_bar, 25, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(_music_bar, lv_color_hex(0x334155), LV_PART_MAIN);
    lv_obj_set_style_bg_color(_music_bar, lv_color_hex(0x38BDF8), LV_PART_INDICATOR);

    // Time elapsed label
    _time_pos_label = lv_label_create(_music_card);
    lv_obj_set_pos(_time_pos_label, 162, 72);
    lv_label_set_text(_time_pos_label, "0:45");
    lv_obj_set_style_text_font(_time_pos_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_time_pos_label, lv_color_hex(0x64748B), 0);
}

void UiManager::buildClimateBlock() {
    // Container: X=4, Y=112, W=214, H=124
    _climate_card = lv_obj_create(_scr);
    lv_obj_set_pos(_climate_card, 4, 112);
    lv_obj_set_size(_climate_card, 214, 124);
    lv_obj_set_style_bg_color(_climate_card, lv_color_hex(0x141C2B), 0);
    lv_obj_set_style_border_color(_climate_card, lv_color_hex(0x232F42), 0);
    lv_obj_set_style_border_width(_climate_card, 1, 0);
    lv_obj_set_style_radius(_climate_card, 10, 0);
    lv_obj_set_style_pad_all(_climate_card, 0, 0);
    lv_obj_clear_flag(_climate_card, LV_OBJ_FLAG_SCROLLABLE);

    // Large Digital Clock
    _clock_label = lv_label_create(_climate_card);
    lv_obj_set_pos(_clock_label, 8, 4);
    lv_label_set_text(_clock_label, "10:45");
    lv_obj_set_style_text_font(_clock_label, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(_clock_label, lv_color_hex(0xFFFFFF), 0);

    // AM/PM Indicator
    _ampm_label = lv_label_create(_climate_card);
    lv_obj_set_pos(_ampm_label, 110, 9);
    lv_label_set_text(_ampm_label, "AM");
    lv_obj_set_style_text_font(_ampm_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_ampm_label, lv_color_hex(0x38BDF8), 0);

    // Date Label
    _date_label = lv_label_create(_climate_card);
    lv_obj_set_pos(_date_label, 110, 24);
    lv_label_set_text(_date_label, "Wed, 23 Sep");
    lv_obj_set_style_text_font(_date_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_date_label, lv_color_hex(0x94A3B8), 0);

    // Subtle divider
    lv_obj_t* sep = lv_obj_create(_climate_card);
    lv_obj_set_pos(sep, 8, 44);
    lv_obj_set_size(sep, 198, 1);
    lv_obj_set_style_bg_color(sep, lv_color_hex(0x232F42), 0);
    lv_obj_set_style_border_width(sep, 0, 0);

    // Temperature Sub-card (Left)
    lv_obj_t* temp_card = lv_obj_create(_climate_card);
    lv_obj_set_pos(temp_card, 6, 49);
    lv_obj_set_size(temp_card, 98, 68);
    lv_obj_set_style_bg_color(temp_card, lv_color_hex(0x0E1420), 0);
    lv_obj_set_style_border_color(temp_card, lv_color_hex(0x1F2B3E), 0);
    lv_obj_set_style_border_width(temp_card, 1, 0);
    lv_obj_set_style_radius(temp_card, 8, 0);
    lv_obj_set_style_pad_all(temp_card, 0, 0);
    lv_obj_clear_flag(temp_card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* temp_tag = lv_label_create(temp_card);
    lv_obj_set_pos(temp_tag, 8, 6);
    lv_label_set_text(temp_tag, "TEMP");
    lv_obj_set_style_text_font(temp_tag, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(temp_tag, lv_color_hex(0xFB923C), 0); // Warm orange

    _temp_val_label = lv_label_create(temp_card);
    lv_obj_set_pos(_temp_val_label, 6, 26);
    lv_label_set_text(_temp_val_label, "24.5 °C");
    lv_obj_set_style_text_font(_temp_val_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(_temp_val_label, lv_color_hex(0xFFFFFF), 0);

    // Humidity Sub-card (Right)
    lv_obj_t* humid_card = lv_obj_create(_climate_card);
    lv_obj_set_pos(humid_card, 110, 49);
    lv_obj_set_size(humid_card, 98, 68);
    lv_obj_set_style_bg_color(humid_card, lv_color_hex(0x0E1420), 0);
    lv_obj_set_style_border_color(humid_card, lv_color_hex(0x1F2B3E), 0);
    lv_obj_set_style_border_width(humid_card, 1, 0);
    lv_obj_set_style_radius(humid_card, 8, 0);
    lv_obj_set_style_pad_all(humid_card, 0, 0);
    lv_obj_clear_flag(humid_card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* humid_tag = lv_label_create(humid_card);
    lv_obj_set_pos(humid_tag, 8, 6);
    lv_label_set_text_fmt(humid_tag, "%s HUMID", LV_SYMBOL_TINT);
    lv_obj_set_style_text_font(humid_tag, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(humid_tag, lv_color_hex(0x38BDF8), 0); // Sky blue

    _humid_val_label = lv_label_create(humid_card);
    lv_obj_set_pos(_humid_val_label, 12, 26);
    lv_label_set_text(_humid_val_label, "58 %");
    lv_obj_set_style_text_font(_humid_val_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(_humid_val_label, lv_color_hex(0xFFFFFF), 0);
}

void UiManager::buildRoomsBlock() {
    // Container: X=222, Y=4, W=94, H=232
    _rooms_card = lv_obj_create(_scr);
    lv_obj_set_pos(_rooms_card, 222, 4);
    lv_obj_set_size(_rooms_card, 94, 232);
    lv_obj_set_style_bg_color(_rooms_card, lv_color_hex(0x141C2B), 0);
    lv_obj_set_style_border_color(_rooms_card, lv_color_hex(0x232F42), 0);
    lv_obj_set_style_border_width(_rooms_card, 1, 0);
    lv_obj_set_style_radius(_rooms_card, 10, 0);
    lv_obj_set_style_pad_all(_rooms_card, 0, 0);
    lv_obj_clear_flag(_rooms_card, LV_OBJ_FLAG_SCROLLABLE);

    // Header Label
    lv_obj_t* header = lv_label_create(_rooms_card);
    lv_label_set_text(header, "ROOMS");
    lv_obj_set_style_text_font(header, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0x64748B), 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 5);

    // 5 Room Buttons
    for (uint8_t i = 0; i < 5; i++) {
        _room_btns[i] = lv_btn_create(_rooms_card);
        lv_obj_set_pos(_room_btns[i], 5, 23 + (i * 41));
        lv_obj_set_size(_room_btns[i], 84, 37);
        lv_obj_set_style_radius(_room_btns[i], 6, 0);
        lv_obj_set_style_border_width(_room_btns[i], 1, 0);
        lv_obj_set_style_pad_all(_room_btns[i], 0, 0);

        _room_labels[i] = lv_label_create(_room_btns[i]);
        lv_label_set_text(_room_labels[i], ROOM_NAMES[i]);
        lv_obj_set_style_text_font(_room_labels[i], &lv_font_montserrat_12, 0);
        lv_obj_center(_room_labels[i]);

        lv_obj_add_event_cb(_room_btns[i], onRoomBtnClicked, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    }
}

void UiManager::selectRoom(uint8_t index) {
    if (index >= 5) return;
    _selected_room_idx = index;

    for (uint8_t i = 0; i < 5; i++) {
        if (i == index) {
            // Selected Room Style
            lv_obj_set_style_bg_color(_room_btns[i], lv_color_hex(0x2563EB), 0); // Vibrant blue
            lv_obj_set_style_border_color(_room_btns[i], lv_color_hex(0x60A5FA), 0);
            lv_obj_set_style_text_color(_room_labels[i], lv_color_hex(0xFFFFFF), 0);
        } else {
            // Unselected Room Style
            lv_obj_set_style_bg_color(_room_btns[i], lv_color_hex(0x1E293B), 0); // Dark slate
            lv_obj_set_style_border_color(_room_btns[i], lv_color_hex(0x334155), 0);
            lv_obj_set_style_text_color(_room_labels[i], lv_color_hex(0xCBD5E1), 0);
        }
    }

    // Update active speaker bubble in Block 1
    if (_speaker_label) {
        lv_label_set_text_fmt(_speaker_label, "%s %s", LV_SYMBOL_VOLUME_MAX, ROOM_NAMES[index]);
    }

    Serial.printf("[UI] Room Selected: %s (Active speaker updated)\n", ROOM_NAMES[index]);
}

void UiManager::updateMusicTrack(uint8_t index) {
    if (index >= NUM_TRACKS) return;
    _current_track_idx = index;
    _track_elapsed_sec = 0;

    lv_label_set_text(_track_title_label, PLAYLIST[index].title);
    lv_label_set_text(_artist_label, PLAYLIST[index].artist);
    lv_bar_set_value(_music_bar, 0, LV_ANIM_OFF);
    lv_label_set_text(_time_pos_label, "0:00");
}

void UiManager::updateTelemetry(const SystemTelemetry& telemetry) {
    (void)telemetry;
    uint32_t now = millis();

    // 1. Digital Clock Tick (Every 1000ms)
    if (now - _last_clock_tick_ms >= 1000) {
        _last_clock_tick_ms = now;
        _clock_secs++;
        if (_clock_secs >= 60) {
            _clock_secs = 0;
            _clock_mins++;
            if (_clock_mins >= 60) {
                _clock_mins = 0;
                _clock_hours++;
                if (_clock_hours > 12) {
                    _clock_hours = 1;
                }
            }
            if (_clock_label) {
                lv_label_set_text_fmt(_clock_label, "%02d:%02d", _clock_hours, _clock_mins);
            }
        }
    }

    // 2. Music Playback Progress Tick (Every 1000ms when playing)
    if (_is_playing && (now - _last_progress_tick_ms >= 1000)) {
        _last_progress_tick_ms = now;
        _track_elapsed_sec++;

        uint16_t duration = PLAYLIST[_current_track_idx].duration_sec;
        if (_track_elapsed_sec >= duration) {
            // Track completed -> advance to next track
            _current_track_idx = (_current_track_idx + 1) % NUM_TRACKS;
            updateMusicTrack(_current_track_idx);
        } else {
            int pct = (_track_elapsed_sec * 100) / duration;
            if (_music_bar) {
                lv_bar_set_value(_music_bar, pct, LV_ANIM_ON);
            }
            if (_time_pos_label) {
                lv_label_set_text_fmt(_time_pos_label, "%d:%02d",
                                      _track_elapsed_sec / 60,
                                      _track_elapsed_sec % 60);
            }
        }
    }
}
