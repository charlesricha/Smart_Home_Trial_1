# 📱 NORA Smart Home Companion App

**AI Assistant:** NORA ("Hey Nora")  
**Theme:** Glassmorphism Dark Theme (`#0B0F19`) with Warm Orange (`#F97316`) & Cyan (`#38BDF8`) Accents  
**Target:** Android / iOS / Web / Desktop  

---

## 🌟 Key Features

1. **Dynamic Multi-Room Architecture**:
   * **Kitchen**: Real-time water valve control (`OPEN TAP` / `CLOSE TAP`), live pulse flow rate (`L/min`), persistent volume (`L`), gas sensor status, and safety leak alarm reset.
   * **Living Room**: Smart blinds control, living TV power, AC climate temperature slider (16°C – 30°C), and ambient light dimmer.
   * **Bedroom**: Master **Sleep Mode** toggle, ceiling fan 4-speed selector, smart curtains, and night lamp.
   * **Dining**: Chandelier brightness, mood lighting presets, and dining room speaker volume.
   * **Bathroom**: Geyser / water heater temperature control (35°C – 65°C), exhaust fan, and vanity mirror light.

2. **Block 1: Music Player & Dynamic Audio Routing**:
   * Synchronized music player with active speaker badge pill (`🔊 Living Room`, `🔊 Kitchen`, etc.) that follows the selected room.
   * Transport controls: Play / Pause, Next, Prev, and animated progress bar.

3. **Live ESP32 Hardware Integration**:
   * Talks directly to your ESP32 node over local WiFi at `http://smarthome-kitchen.local` or direct IP.
   * Automatically polls `/api/status` every 1000ms.
   * Dispatches thread-safe REST commands to `/api/valve`.

4. **NORA Voice Trigger**:
   * Floating Action Button with voice modal simulation for *"Hey Nora"*.

---

## 🚀 Running the App

Once Flutter is installed in your PATH:

```bash
cd mobile_app
flutter pub get
flutter run
```

To run as a web app on your laptop:
```bash
flutter run -d chrome
```

To connect to your ESP32:
* Tap the connection badge in the top-right corner of the app bar.
* Enter your ESP32 IP address (e.g. `http://192.168.1.50` or `http://smarthome-kitchen.local`).
