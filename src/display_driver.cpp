/**
 * @file display_driver.cpp
 * @brief Implementation of display & touch drivers and LVGL HAL interfacing
 */

#include "display_driver.h"

LGFX_VIEWE tft;

// LVGL draw buffers (allocated in internal SRAM for maximum DMA throughput)
#define DISP_BUF_LINES 30
static lv_color_t buf_1[LCD_WIDTH * DISP_BUF_LINES];
static lv_color_t buf_2[LCD_WIDTH * DISP_BUF_LINES];
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

LGFX_VIEWE::LGFX_VIEWE() {
    {
        auto cfg = _bus_instance.config();
        cfg.spi_host = SPI2_HOST;
        cfg.spi_mode = 0;
        cfg.freq_write = 40000000; // 40MHz for fast rendering
        cfg.freq_read  = 16000000;
        cfg.spi_3wire  = false;
        cfg.use_lock   = true;
        cfg.dma_channel = SPI_DMA_CH_AUTO;
        cfg.pin_sclk   = PIN_LCD_SCLK;
        cfg.pin_mosi   = PIN_LCD_MOSI;
        cfg.pin_miso   = -1;
        cfg.pin_dc     = PIN_LCD_DC;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
    }

    {
        auto cfg = _panel_instance.config();
        cfg.pin_cs           = PIN_LCD_CS;
        cfg.pin_rst          = PIN_LCD_RST;
        cfg.pin_busy         = -1;
        cfg.panel_width      = LCD_WIDTH;
        cfg.panel_height     = LCD_HEIGHT;
        cfg.offset_x         = 0;
        cfg.offset_y         = 0;
        cfg.offset_rotation  = 4; // Bit 2 set: LovyanGFX vertical flip (MAD_MY)
        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits  = 1;
        cfg.readable         = false;
        cfg.invert           = false; // UEDX24320028E uses non-inverted colors
        cfg.rgb_order        = true;  // BGR color format
        cfg.dlen_16bit       = false;
        cfg.bus_shared       = false;
        _panel_instance.config(cfg);
    }

    {
        auto cfg = _touch_instance.config();
        cfg.x_min      = 0;
        cfg.x_max      = LCD_WIDTH - 1;
        cfg.y_min      = 0;
        cfg.y_max      = LCD_HEIGHT - 1;
        cfg.pin_int    = -1; // -1 to poll I2C directly without relying on external INT pin
        cfg.pin_rst    = PIN_TOUCH_RST; // GPIO 2
        cfg.bus_shared = false;
        cfg.offset_rotation = 0;
        cfg.i2c_port   = 0;  // I2C0 host
        cfg.i2c_addr   = 0x2E; // Standard CHSC6540 I2C address
        cfg.pin_sda    = PIN_TOUCH_SDA; // GPIO 1
        cfg.pin_scl    = PIN_TOUCH_SCL; // GPIO 3
        cfg.freq       = 400000;
        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
    }

#if (PIN_LCD_BL >= 0)
    {
        auto cfg = _light_instance.config();
        cfg.pin_bl = PIN_LCD_BL;
        cfg.invert = false;
        cfg.freq   = 44100;
        cfg.pwm_channel = 7;
        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance);
    }
#endif

    setPanel(&_panel_instance);
}

// Display flushing callback called by LVGL
static void disp_flush_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixelsDMA((lgfx::rgb565_t*)&color_p->full, w * h);
    tft.waitDMA();
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// Touch input read callback called by LVGL
static void touchpad_read_cb(lv_indev_drv_t* indev, lv_indev_data_t* data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY);

    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
        Serial.printf("[TOUCH] Screen pressed at X=%d, Y=%d\n", touchX, touchY);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

bool display_driver_init() {
    Serial.println("[HAL] Configuring GC9307 mode select strapping pins (IM0=0, IM1=1)...");
    pinMode(PIN_LCD_IM0, OUTPUT);
    pinMode(PIN_LCD_IM1, OUTPUT);
    digitalWrite(PIN_LCD_IM0, LOW);
    digitalWrite(PIN_LCD_IM1, HIGH);
    delay(20);

    Serial.println("[HAL] Resetting CHSC6540 touch controller (GPIO 2)...");
    pinMode(PIN_TOUCH_RST, OUTPUT);
    digitalWrite(PIN_TOUCH_RST, LOW);
    delay(10);
    digitalWrite(PIN_TOUCH_RST, HIGH);
    delay(50);

    pinMode(PIN_TOUCH_SDA, INPUT_PULLUP);
    pinMode(PIN_TOUCH_SCL, INPUT_PULLUP);

    Serial.println("[HAL] Initializing LovyanGFX display and touch...");
    if (!tft.init()) {
        Serial.println("[HAL] ERROR: LovyanGFX initialization failed!");
        return false;
    }

    tft.setRotation(0); // Portrait (240x320)
    tft.setBrightness(255);

    // Ensure backlight pin is driven HIGH
    pinMode(PIN_LCD_BL, OUTPUT);
    digitalWrite(PIN_LCD_BL, HIGH);

    tft.fillScreen(TFT_BLACK);

    Serial.println("[HAL] Initializing LVGL core...");
    lv_init();

    // Setup draw buffer
    lv_disp_draw_buf_init(&draw_buf, buf_1, buf_2, LCD_WIDTH * DISP_BUF_LINES);

    // Register display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;
    disp_drv.flush_cb = disp_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Register touch input driver
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read_cb;
    lv_indev_drv_register(&indev_drv);

    Serial.println("[HAL] LVGL Display & Touch driver initialized successfully.");
    return true;
}
