# 📘 Smart Home Node: Beginner's Complete Project & Code Guide

Welcome! This guide explains **what every file in this repository does**, **how to change settings (like your WiFi password)**, and **how to build, upload, and monitor your code in VS Code**.

---

## 🧭 Quick Reference: How to Change WiFi Credentials

If your WiFi name (SSID) or password changes, you only need to edit **one file**:

👉 **[`include/config.h`](file:///home/chaloo/Smart_Home/include/config.h)** (around lines 87–90):

```cpp
// Set your local WiFi credentials here:
#define DEFAULT_WIFI_SSID       "YourNewWiFiName"
#define DEFAULT_WIFI_PASS       "YourNewWiFiPassword"
#define WIFI_CONNECT_TIMEOUT_MS 10000
```

1. Open `include/config.h`.
2. Change the text between the quotes `"..."`.
3. Save the file (`Ctrl + S`).
4. Re-upload to the board (click the **`→`** arrow at the bottom of VS Code).

---

## 📂 Project Directory Map

Here is the entire layout of this repository and the role of each file:

```text
Smart_Home/
├── platformio.ini              # ⚙️ PlatformIO Build Settings (Libraries, Board Type, Memory)
├── BOARD_SPEC.md               # 📋 Hardware Pinouts & Display/Touch chip specifications
├── MILESTONES.md               # 🎯 Project roadmap and progress checklist
├── GUIDE.md                    # 📘 This guide!
│
├── include/                    # 📑 C++ Header Files (Declarations & Config)
│   ├── config.h                # 🔑 MASTER CONFIG: WiFi, Pins, Servo Angles, Safety Limits
│   ├── types.h                 # 📦 Shared Data Structures: ValveState, Telemetry, Commands
│   ├── lv_conf.h               # 🎨 LVGL Graphics Configuration (Fonts, Colors, Buffers)
│   ├── display_driver.h        # 🖥️ LovyanGFX Display & CHSC6540 Capacitive Touch Driver
│   ├── ui_manager.h            # 📱 LVGL UI Layout (Music card, Clock/Climate, Rooms menu)
│   ├── network_manager.h       # 🌐 WiFi, mDNS, REST API & Web Dashboard Headers
│   ├── flow_sensor.h           # 💧 YF-S201 Flow Sensor pulse counting & NVS storage
│   ├── valve_controller.h      # 🚰 SG90 Servo PWM Driver & Auto-Detach timer
│   └── safety_supervisor.h     # 🛡️ Flood protection, Leak detection & Auto-shutoff
│
└── src/                        # 💻 C++ Implementation Files (Executable Code)
    ├── main.cpp                # 🧠 Main FreeRTOS Orchestrator & Task Spawner
    ├── display_driver.cpp      # 🖥️ Hardware init for GC9307 LCD & CHSC6540 I2C Touch
    ├── ui_manager.cpp          # 📱 Builds and animates the 320x240 Landscape UI
    ├── network_manager.cpp     # 🌐 Handles WiFi connection, REST API & Web Dashboard
    ├── flow_sensor.cpp         # 💧 Pulse ISR, Flow rate calculation, Flash memory saving
    ├── valve_controller.cpp    # 🚰 Controls servo motor angles (0° = Closed, 90° = Open)
    └── safety_supervisor.cpp   # 🛡️ Monitors flow timeouts and triggers safety alarms
```

---

## 🔍 Detailed Breakdown of Every File

### 1. [`include/config.h`](file:///home/chaloo/Smart_Home/include/config.h) *(Most Important!)*
This is the single source of truth for all parameters. If you ever need to tweak hardware behavior, look here first:
* **WiFi Settings:** `DEFAULT_WIFI_SSID` & `DEFAULT_WIFI_PASS`.
* **Hardware Pinout:**
  * Flow Sensor pin: `PIN_FLOW_SENSOR` (GPIO 10)
  * Servo motor PWM pin: `PIN_SERVO_PWM` (GPIO 14)
  * LCD Backlight: `PIN_LCD_BL` (GPIO 13)
* **Servo Angles:**
  * `SERVO_ANGLE_CLOSED` (`0` degrees)
  * `SERVO_ANGLE_OPEN` (`90` degrees)
* **Safety Thresholds:**
  * `SAFETY_MAX_CONTINUOUS_SEC` (default: 900s / 15 minutes before emergency shutoff)
  * `SAFETY_MAX_SESSION_VOL_L` (default: 50.0 Liters before auto-shutoff)
  * `SAFETY_LEAK_THRESH_LPM` (triggers leak alarm if water flows while valve is closed)

---

### 2. [`src/main.cpp`](file:///home/chaloo/Smart_Home/src/main.cpp) *(The Brain)*
This is where the microcontroller boots up (`setup()`) and runs. It uses a **Dual-Core FreeRTOS Architecture**:
* **Core 0 (Task 1: `ControlTask`):** High-priority task that reads the water sensor, moves the servo motor, and supervises safety limits.
* **Core 0 (Task 2: `NetworkTask`):** Listens for incoming WiFi web requests (`/api/valve`, `/api/status`) and serves the web dashboard.
* **Core 1 (Task 3: `UiTask`):** Runs the LVGL graphics engine and capacitive touch scanning at a smooth 60 frames per second.
* **`loop()`:** Listens to the USB Serial console for keyboard commands (`o` = open, `c` = close, `t` = toggle).

---

### 3. [`include/network_manager.h`](file:///home/chaloo/Smart_Home/include/network_manager.h) & [`src/network_manager.cpp`](file:///home/chaloo/Smart_Home/src/network_manager.cpp)
Handles all communications over the network:
* Connects to your home WiFi.
* If WiFi is not reachable, starts an emergency Access Point (`SmartHome-Node-XXXX`, password: `12345678`) at `192.168.4.1`.
* Sets up mDNS so you can visit `http://smarthome-kitchen.local`.
* Serves the **built-in web dashboard** (the dark-theme web page with buttons and live telemetry).
* Exposes the REST API for your upcoming Flutter mobile app.

---

### 4. [`include/ui_manager.h`](file:///home/chaloo/Smart_Home/include/ui_manager.h) & [`src/ui_manager.cpp`](file:///home/chaloo/Smart_Home/src/ui_manager.cpp)
Builds the user interface on the 2.8" LCD screen using the LVGL graphics library:
* **Block 1 (Top-Left):** Music player card (track title, artist, play/pause buttons, speaker pill badge).
* **Block 2 (Bottom-Left):** Digital clock, AM/PM, calendar date, temperature, and humidity.
* **Block 3 (Right Column):** Interactive rooms list (`Living Room`, `Kitchen`, `Dining`, `Bedroom`, `Bathroom`).

---

### 5. [`include/display_driver.h`](file:///home/chaloo/Smart_Home/include/display_driver.h) & [`src/display_driver.cpp`](file:///home/chaloo/Smart_Home/src/display_driver.cpp)
Low-level hardware driver for the VIEWE board:
* Configures the **GC9307** SPI display via LovyanGFX.
* Implements the custom I2C packet decoder for the **CHSC6540** capacitive touch controller.
* Controls the LCD backlight transistor on GPIO 13.

---

### 6. [`include/flow_sensor.h`](file:///home/chaloo/Smart_Home/include/flow_sensor.h) & [`src/flow_sensor.cpp`](file:///home/chaloo/Smart_Home/src/flow_sensor.cpp)
* Counts electrical pulses from the YF-S201 flow sensor via a hardware interrupt (`ISR`).
* Converts pulse frequency to Liters per Minute (L/min).
* Saves cumulative lifetime water usage to non-volatile flash memory (NVS) so totals are never lost when powered off.

---

### 7. [`include/valve_controller.h`](file:///home/chaloo/Smart_Home/include/valve_controller.h) & [`src/valve_controller.cpp`](file:///home/chaloo/Smart_Home/src/valve_controller.cpp)
* Generates 50Hz PWM pulses using the ESP32 LEDC hardware timer.
* Moves the SG90 micro-servo to 0° (Closed) or 90° (Open).
* **Auto-Detach:** Automatically shuts off the PWM signal 700ms after moving. This prevents servo buzzing, thermal wear, and brownouts.

---

### 8. [`include/safety_supervisor.h`](file:///home/chaloo/Smart_Home/include/safety_supervisor.h) & [`src/safety_supervisor.cpp`](file:///home/chaloo/Smart_Home/src/safety_supervisor.cpp)
Acts as an automated safety watchdog:
* If the tap is left running for more than 15 minutes, it forces the valve shut.
* If more than 50 Liters pass in a single session, it forces the valve shut.
* If water flows while the valve is supposed to be closed, it triggers a **LEAK DETECTED** alert.

---

### 9. [`platformio.ini`](file:///home/chaloo/Smart_Home/platformio.ini)
The project configuration file for PlatformIO:
* Tells PlatformIO which board you have (`esp32-s3-devkitc-1`).
* Configures 16MB Flash and 8MB Octal PSRAM.
* Lists all external libraries (`LovyanGFX`, `lvgl`, `ArduinoJson`).

---

## 🛠️ How to Work in VS Code (Cheat Sheet)

### The Bottom Blue Status Bar:
At the very bottom of your VS Code window, you have quick-action icons:
* **`✓` (Checkmark):** **Build/Compile** code (checks for syntax/compiler errors).
* **`→` (Right Arrow):** **Upload** firmware to the plugged-in ESP32.
* **`🗑` (Trash Can):** **Clean** build files.
* **`🔌` (Plug / Screen):** **Open Serial Monitor** (view debug logs at 115200 baud).

### Terminal Commands (Alternative):
If you prefer typing in the terminal (`Ctrl + ~`):
* Build: `pio run`
* Upload: `pio run --target upload`
* Serial Monitor: `pio device monitor`
* Upload AND Open Monitor in one command: `pio run -t upload -t monitor`
* *(Exit the serial monitor in terminal: Press `Ctrl + C` or `Ctrl + ]`)*

---

## 🧪 Testing the Web Dashboard After Uploading

Once the board boots and connects to your WiFi:
1. Open any browser on your phone or laptop (connected to the same WiFi).
2. Go to:
   ```text
   http://smarthome-kitchen.local
   ```
   *(Or enter the board's numeric IP address, e.g. `http://192.168.1.50`, shown in the Serial Monitor).*
3. You will see your dark-theme dashboard with live water flow, total volume, and buttons to **Open** or **Close** the tap!
