# Smart Home System: Project Milestones & Roadmap

**System Codename:** Project Lyra / Nova  
**Target Hardware:** VIEWE UEDX24320028E-WB-A (ESP32-S3) + Flutter Mobile Companion App  
**Architecture:** Dual-Core FreeRTOS Firmware + Flutter App + Local Network (mDNS/REST) + Firebase Cloud Sync + Voice Agent  

---

## 🎯 Progress Dashboard

- [x] **Milestone 0: Hardware Foundation & Core Actuation** *(Completed)*
- [ ] **Milestone 1: ESP32 Local Networking & Direct API**
- [ ] **Milestone 2: Mobile App Foundation (Flutter Dashboard)**
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

### Milestone 1: ESP32 Local Networking & Direct API
- [ ] Connect ESP32 to local WiFi (`WiFiMulti` or fallback AP mode).
- [ ] Set up mDNS responder (`esp32-node.local` / `smart-valve.local`) to eliminate hardcoded IPs.
- [ ] Implement REST API endpoints:
  - `GET /api/status`: Returns JSON with telemetry (valve state, flow rate, volume, temp).
  - `POST /api/valve`: Body `{"state": "open" | "close"}` to actuate servo.
- [ ] Implement WebSocket or SSE (Server-Sent Events) for real-time telemetry streaming.
- [ ] Verify local API control using curl / Postman.

---

### Milestone 2: Mobile App Foundation (Flutter Dashboard)
- [ ] Initialize Flutter project in `mobile_app/`.
- [ ] Implement Home Screen:
  - Overall status cards (temperature, humidity, active water flow).
  - Quick action widgets.
- [ ] Implement Rooms Screen:
  - Room cards (Living Room, Kitchen, Dining, Bedroom, Bathroom).
  - Per-room device lists and toggle switches.
- [ ] Implement Local Network Discovery / Controller:
  - Query mDNS / local IP for ESP32 nodes.
  - Issue direct REST calls over local WiFi for instant Phase 1 control.

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
