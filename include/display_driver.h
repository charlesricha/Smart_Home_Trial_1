/**
 * @file display_driver.h
 * @brief LovyanGFX hardware abstraction and LVGL 8.x HAL bridge
 * Board: VIEWE UEDX24320028E (GC9307 SPI + CHSC6540 I2C Touch)
 */

#pragma once

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <lgfx/v1/panel/Panel_GC9A01.hpp>
#include <lgfx/v1/touch/Touch_CHSC6540.hpp>
#include <lvgl.h>
#include "config.h"

class Panel_VIEWE_GC9307 : public lgfx::Panel_GC9xxx {
public:
    Panel_VIEWE_GC9307() {
        _cfg.panel_width  = _cfg.memory_width  = 240;
        _cfg.panel_height = _cfg.memory_height = 320;
        _cfg.dummy_read_pixel = 16;
        _nop_closing = false;
    }
protected:
    const uint8_t* getInitCommands(uint8_t listno) const override {
        static constexpr uint8_t list0[] = {
            0xfe, 0,
            0xfe, 0,
            0xef, 0,
            0x86, 1, 0x98,
            0x89, 1, 0x03,
            0x8b, 1, 0x80,
            0x8d, 1, 0x33,
            0x8e, 1, 0x0f,
            0xe8, 2, 0x12, 0x00,
            0xc3, 1, 0x1d,
            0xc4, 1, 0x1d,
            0xc9, 1, 0x0f,
            0xff, 1, 0x62,
            0x99, 1, 0x3e,
            0x9d, 1, 0x4b,
            0x98, 1, 0x3e,
            0x9c, 1, 0x4b,
            0xf0, 6, 0x49, 0x0b, 0x09, 0x08, 0x06, 0x2e,
            0xf2, 6, 0x49, 0x0b, 0x09, 0x08, 0x06, 0x2e,
            0xf1, 6, 0x45, 0x92, 0x93, 0x2b, 0x31, 0x6f,
            0xf3, 6, 0x45, 0x92, 0x93, 0x2b, 0x31, 0x6f,
            0x35, 1, 0x00,
            0x11, 0 + CMD_INIT_DELAY, 120,
            0x29, 0,
            0x2c, 0,
            0xff, 0xff
        };
        switch (listno) {
            case 0: return list0;
            default: return nullptr;
        }
    }
};

class Touch_CHSC6540_VIEWE : public lgfx::ITouch {
public:
    Touch_CHSC6540_VIEWE() {
        _cfg.i2c_addr = 0x2E;
        _cfg.x_min = 0;
        _cfg.x_max = 239;
        _cfg.y_min = 0;
        _cfg.y_max = 319;
    }

    bool init(void) override {
        _inited = lgfx::i2c::init(_cfg.i2c_port, _cfg.pin_sda, _cfg.pin_scl).has_value();
        return _inited;
    }

    void wakeup(void) override {}
    void sleep(void) override {}

    uint_fast8_t getTouchRaw(lgfx::touch_point_t* tp, uint_fast8_t count) override {
        if (tp) tp->size = 0;
        if (!_inited || count == 0) return 0;

        uint8_t buf[15] = {0};
        uint8_t reg = 0x00;

        auto res = lgfx::i2c::transactionWriteRead(_cfg.i2c_port, _cfg.i2c_addr, &reg, 1, buf, sizeof(buf), _cfg.freq);
        if (res.has_value()) {
            uint8_t num = buf[2] & 0x0F;
            if (num > 0) {
                uint16_t raw_x = ((buf[3] & 0x0F) << 8) | buf[4];
                uint16_t raw_y = ((buf[5] & 0x0F) << 8) | buf[6];

                tp[0].size = 1;
                tp[0].id = 0;
                tp[0].x = raw_x;
                tp[0].y = raw_y;
                return 1;
            }
        }
        return 0;
    }
};

class LGFX_VIEWE : public lgfx::LGFX_Device {
    Panel_VIEWE_GC9307     _panel_instance;
    lgfx::Bus_SPI          _bus_instance;
    Touch_CHSC6540_VIEWE   _touch_instance;
#if (PIN_LCD_BL >= 0)
    lgfx::Light_PWM        _light_instance;
#endif

public:
    LGFX_VIEWE();
};

extern LGFX_VIEWE tft;

/**
 * @brief Initialize LovyanGFX display, touch, and register with LVGL
 * @return true on successful initialization
 */
bool display_driver_init();
