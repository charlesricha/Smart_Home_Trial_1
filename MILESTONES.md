# Smart Home System: Project Milestones & Roadmap

**System Codename:** Project NORA ("Hey Nora")  
**Target Hardware:** VIEWE UEDX24320028E-WB-A (ESP32-S3) + Flutter Mobile Companion App  
**Architecture:** Dual-Core FreeRTOS Firmware + Flutter App + Local Network (mDNS/REST) + Firebase Cloud Sync + Voice Agent  

---

## 🎯 Progress Dashboard

- [x] **Milestone 0: Hardware Foundation & Core Actuation** *(Completed)*
- [x] **Milestone 1: ESP32 Local Networking & Direct API** *(Completed)*
- [x] **Milestone 2: Mobile App Foundation (Flutter Companion App)** *(Completed)*
- [ ] **Milestone 3: Cloud Synchronization & Multi-Room Scaling (Firebase)**
- [ ] **Milestone 4: Voice Pipeline (Wake Word + STT + Intent Engine)**
- [ ] **Milestone 5: Music & Media Streaming Agent**
- [ ] **Milestone 6: System Hardening & Polish**

---

## Detailed Milestone Breakdown

### Milestone 0: Hardware Foundation & Core Actuation ✅
- [x] Bring up GC9307 SPI Display on ESP32-S3 (with IM0/IM1 mode strapping).
- [x] Resolve vertical mirroring and LovyanGFX panel offset bugs.
- [x] Implement vendor I2C frame driver for CHSC6540 capacitive touch (`0x2E`).
- [x] Integrate SG90 servo valve control with auto-detach to prevent jitter/brownouts.
- [x] Integrate YF-S201 flow sensor with interrupt-driven pulse counting.
- [x] Set up dual-core FreeRTOS architecture (Core 0: Control/Sensors, Core 1: LVGL UI).
- [x] Create comprehensive hardware specification ([`BOARD_SPEC.md`](BOARD_SPEC.md)).
- [x] Design 3-block landscape UI (Audio, Climate/Time, Room selector).

---

### Milestone 1: ESP32 Local Networking & Direct API ✅
- [x] Connect ESP32 to local WiFi with automatic SoftAP fallback mode.
- [x] Set up mDNS responder (`http://smarthome-kitchen.local`) to eliminate hardcoded IPs.
- [x] Implement REST API endpoints:
  - `GET /api/status`: Returns JSON with telemetry (valve state, flow rate, volume, alarms, uptime, RSSI).
  - `GET /api/valve` and `POST /api/valve`: Query param or JSON body `{"state": "open" | "close" | "toggle"}` to actuate servo via thread-safe FreeRTOS `control_queue`.
  - `POST /api/alarm/reset`: Clears active safety alarms.
  - `POST /api/volume/reset`: Resets session/total flow counter.
  - `GET /api/device`: Node registration and capability discovery.
  - `GET /api/ping`: Fast health check.
- [x] Implement responsive Dark-Theme Web Dashboard (`GET /`) embedded directly into firmware.
- [x] Pin NetworkTask to Core 0 (priority 1) to keep Core 1 UI 60 FPS smooth.

---

### Milestone 2: Mobile App Foundation (Flutter Companion App) ✅
- [x] Initialize Flutter project in `mobile_app/`.
- [x] Implement Master Home Screen (`home_dashboard.dart`):
  - Digital clock, date, indoor temperature & humidity summary bar.
  - ESP32 live connection badge (`🟢 Online` / `🔴 Offline`) with IP setup dialog.
- [x] Implement Block 1: Music Player & Dynamic Speaker Routing (`music_player_card.dart`):
  - Pill badge tracking active selected room (`🔊 Kitchen`, `🔊 Living Room`, etc.).
  - Transport controls (Play/Pause, Next, Prev) with animated progress bar.
- [x] Implement Dynamic Multi-Room Screen Architecture:
  - **Kitchen** (`kitchen_view.dart`): Valve tap control (`OPEN/CLOSE TAP`), live pulse flow rate, total volume, gas sensor, leak alarms.
  - **Living Room** (`living_room_view.dart`): Smart blinds, TV power, AC climate temperature slider, ambient lights.
  - **Bedroom** (`bedroom_view.dart`): Master Sleep Mode, ceiling fan 4-speed selector, curtains, night lamp.
  - **Dining** (`dining_view.dart`): Chandelier dimmer, mood scene preset, dining room speaker volume.
  - **Bathroom** (`bathroom_view.dart`): Geyser / water heater temperature control (35°C–65°C), exhaust fan, vanity mirror.
- [x] Implement Local Network REST Controller (`esp32_service.dart`):
  - Auto-polls `/api/status` at 1000ms.
  - Dispatches commands to `/api/valve`.
- [x] Add NORA Floating Voice Trigger button with voice listening modal simulation.


---

### Milestone 3: Cloud Synchronization & Multi-Room Scaling (Firebase)
- [ ] Create Firebase project (Realtime Database / Firestore).
- [ ] Define database schema for `/rooms/` and `/devices/`.
- [ ] Add `Firebase-ESP-Client` to ESP32 PlatformIO configuration.
- [ ] Implement ESP32 auto-registration on boot (MAC address -> unique device ID).
- [ ] Implement live stream listener on ESP32 for `/devices/{id}/command`.
- [ ] Connect Flutter app to Firebase (`flutterfire`) so device state updates remotely in real time.

---

### Milestone 4: Voice Pipeline (Wake Word + STT + Intent Engine)
- [ ] Integrate **Picovoice Porcupine** into Flutter app (`porcupine_flutter`) for offline wake word (e.g., *"Hey Lyra"* or *"Hey Nova"*).
- [ ] Configure Android `ForegroundService` so wake-word detection runs when app is minimized.
- [ ] Wire up Speech-to-Text (Android Native `SpeechRecognizer` on wake trigger).
- [ ] Build Tier-1 Regex / Slot-Filling Intent Parser:
  - Extract: `[action]` (open/close/set) + `[room]` (kitchen/living room) + `[device]` (tap/light/fan).
- [ ] Build Tier-2 LLM Fallback (Groq Llama-3 or Gemini Flash free tier) for complex phrasing.
- [ ] Route resolved commands directly to Firebase/Local API.

---

### Milestone 5: Music & Media Streaming Agent
- [ ] Select free audio provider:
  - Radio Browser API (35k+ live stations), OR
  - Piped / Invidious API (YouTube audio stream extraction), OR
  - Spotify App Remote SDK (phone Spotify control).
- [ ] Create voice command triggers (*"Play some Lo-Fi"*, *"Pause music"*, *"Next track"*).
- [ ] Stream audio to phone speaker or route audio metadata to ESP32 display widget.

---

### Milestone 6: System Hardening & Polish
- [ ] Add offline fallback logic (if internet goes down, fallback to local WiFi mDNS).
- [ ] Implement OTA (Over-The-Air) firmware updates for ESP32 nodes.
- [ ] Add safety watchdog alerts to phone notifications (e.g. continuous water leak warning).
- [ ] Build custom 3D enclosure for the VIEWE display wall mount.
