# Hardware Specification: VIEWE UEDX24320028E-WB-A

**Target Board:** VIEWE UEDX24320028E-WB-A (Smart Display Module)  
**Microcontroller:** ESP32-S3-WROOM-1-N16R8 (Dual-Core Xtensa LX7 @ 240MHz)  
**Memory:** 16 MB Quad SPI Flash (`qio`), 8 MB Octal PSRAM (`opi`)  
**Flash Mode / Freq:** `qio`, 80 MHz | **PSRAM:** Octal SPI (OPI) 80 MHz

---

## 1. On-Board Pinout & Peripherals Reference

### A. Display (2.8" IPS TFT 240x320, Driver: GC9307 / GC9A01-family)
| Signal | ESP32-S3 GPIO | Function / Notes |
| :--- | :---: | :--- |
| **LCD_CS** | **GPIO 42** | SPI Chip Select (Active LOW) |
| **LCD_SCLK** | **GPIO 40** | SPI Clock |
| **LCD_MOSI** | **GPIO 45** | SPI Data Out (MOSI) |
| **LCD_DC** | **GPIO 41** | Data / Command Control |
| **LCD_RST** | **GPIO 39** | Hardware Reset (or tied to system EN) |
| **LCD_BL** | **GPIO 13** | **Backlight LED Enable / PWM** (Active HIGH)<br>⚠️ **CRITICAL: NEVER use GPIO 13 for external sensors or servos!** |
| **IM0** | **GPIO 47** | GC9307 Interface Mode Strap (Must drive **LOW / 0** for 4-wire SPI) |
| **IM1** | **GPIO 48** | GC9307 Interface Mode Strap (Must drive **HIGH / 1** for 4-wire SPI) |

> ⚠️ **CRITICAL DISPLAY HARDWARE RULES:**
> 1. **IM0 & IM1 Strapping:** The GC9307 controller requires **GPIO 47 = LOW** and **GPIO 48 = HIGH** prior to `tft.init()`, otherwise the chip will not accept 4-wire SPI commands.
> 2. **Backlight Transistor:** GPIO 13 directly controls the backlight driver transistor. It must be configured as an output and driven `HIGH` (or PWM) to illuminate the screen.
> 3. **LovyanGFX Panel Class:** LovyanGFX's default `Panel_GC9307` has a known `+34px` window offset bug designed for narrow 172x320 displays. Use `Panel_VIEWE_GC9307` derived from `lgfx::Panel_GC9xxx` with the board's vendor init commands, `invert = false`, `rgb_order = true` (BGR), and `offset_rotation = 4` (upright portrait).

---

### B. Capacitive Touch (CHSC6540 Controller via I2C)
| Signal | ESP32-S3 GPIO | Function / Notes |
| :--- | :---: | :--- |
| **TOUCH_SDA** | **GPIO 1** | I2C Data (400 kHz, enable internal pullup) |
| **TOUCH_SCL** | **GPIO 3** | I2C Clock (400 kHz, enable internal pullup) |
| **TOUCH_INT** | **GPIO 4** | Touch Interrupt. Set `pin_int = -1` in driver to avoid INT gating |
| **TOUCH_RST** | **GPIO 2** | Hardware Reset. Must pulse LOW (10ms) -> HIGH (50ms) before init |
| **I2C Host** | **I2C Port 0** | **Must use `i2c_port = 0`**, not port 1 |
| **I2C Address** | **0x2E** | Verified active responding address via I2C scan |

> ⚠️ **CRITICAL TOUCH DRIVER REQUIREMENTS:**
> 1. **Why LovyanGFX `Touch_CHSC6540` Fails:** The stock LovyanGFX driver sends an unsupported command `{0x5A, 0x5A}` and polls register `0x02`. It also aborts reads if `pin_int` is HIGH.
> 2. **Working Packet Protocol (`Touch_CHSC6540_VIEWE`):**
>    - Issue a single I2C Write-Read transaction: Write 1 byte (`0x00`), Read 15 bytes.
>    - `buf[2] & 0x0F`: Number of active touch contacts (0 = released, 1 = pressed).
>    - `((buf[3] & 0x0F) << 8) | buf[4]`: Raw X coordinate [0 .. 239].
>    - `((buf[5] & 0x0F) << 8) | buf[6]`: Raw Y coordinate [0 .. 319].
> 3. **Hardware Reset:** Always toggle `GPIO 2` LOW for 10ms then HIGH for 50ms before initializing the I2C bus to ensure the controller exits low-power shutdown.

---

### C. Other On-Board Peripherals
| Peripheral | ESP32-S3 GPIO | Notes |
| :--- | :---: | :--- |
| **WS2812 RGB LED** | **GPIO 0** | Also tied to BOOT button strapping |
| **Passive Buzzer** | **GPIO 38** | PWM Tone output |
| **USB CDC / JTAG** | **GPIO 19, 20** | Native USB D- / D+ |
| **UART0 Debug** | **TX: GPIO 43, RX: GPIO 44** | On-board CH340C USB-to-UART bridge |
| **MicroSD Card** | **GPIO 15, 16, 17, 18** | SDIO / SPI Data and Clock lines |

---

### D. Safe External Expansion GPIOs (Header Connections)
When wiring external sensors (e.g. flow meters) or actuators (e.g. servos), use the following available GPIOs that have no conflicts with on-board hardware:
- **GPIO 10** *(Assigned to Flow Sensor)*
- **GPIO 14** *(Assigned to SG90 Servo PWM)*
- **GPIO 11** *(Available)*
- **GPIO 12** *(Available)*
- **GPIO 21** *(Available on some header revisions)*

---

## 2. PlatformIO Configuration (`platformio.ini`)
```ini
[env:viewe_esp32s3_node]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

board_build.mcu = esp32s3
board_build.f_cpu = 240000000L
board_build.f_flash = 80000000L
board_build.flash_mode = qio
board_build.flash_size = 16MB
board_build.arduino.memory_type = qio_opi
board_upload.flash_size = 16MB
board_build.partitions = default_16MB.csv

build_flags =
    -DBOARD_HAS_PSRAM
    -mfix-esp32-psram-cache-issue
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DARDUINO_USB_MODE=1
    -DLV_CONF_INCLUDE_SIMPLE
    -Iinclude
    -O2

lib_deps =
    lovyan03/LovyanGFX @ ^1.2.0
    lvgl/lvgl @ ^8.3.11
```

---

## 3. Water Control Project Pin Mapping Summary

| Subsystem | Signal | Board GPIO | Level / Mode |
| :--- | :--- | :---: | :--- |
| **Flow Sensor** | Pulse Input | **GPIO 10** | 3.3V Logic (5V level-shifted), `INPUT_PULLUP` |
| **Servo Actuator**| PWM Control | **GPIO 14** | 50 Hz PWM (500us to 2400us), auto-detach |
| **LCD Backlight** | Enable / PWM | **GPIO 13** | Output HIGH (100% duty) |
| **LCD Mode 0** | Strapping IM0 | **GPIO 47** | Output LOW (0) |
| **LCD Mode 1** | Strapping IM1 | **GPIO 48** | Output HIGH (1) |
| **LCD SPI** | SCK / MOSI / CS / DC | **40 / 45 / 42 / 41** | 40-80 MHz SPI bus |
| **Touch I2C** | SDA / SCL / INT / RST | **1 / 3 / 4 / 2** | 400 kHz Fast I2C (Address 0x2E) |
