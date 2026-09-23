/**
 * @file network_manager.cpp
 * @brief ESP32 Local Networking, mDNS, and REST API Implementation
 */

#include "network_manager.h"

NetworkManager networkManager;

// Embedded Full-Screen Exact-Replica Dashboard (Mirrors ESP32 Landscape UI)
static const char HTML_DASHBOARD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Home - Dashboard</title>
  <style>
    :root {
      --bg: #0B0F19;
      --card: #141C2B;
      --card-sub: #0E1420;
      --border: #232F42;
      --border-sub: #1F2B3E;
      --accent: #2563EB;
      --accent-glow: #60A5FA;
      --cyan: #38BDF8;
      --emerald: #10B981;
      --crimson: #EF4444;
      --amber: #F59E0B;
      --orange: #FB923C;
      --text: #FFFFFF;
      --text-dim: #94A3B8;
      --text-muted: #64748B;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
    body {
      background: var(--bg);
      color: var(--text);
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      padding: 16px;
    }
    .dashboard-wrapper {
      width: 100%;
      max-width: 960px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }
    .top-bar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 8px 16px;
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 10px;
      font-size: 0.85rem;
    }
    .top-bar span { color: var(--text-dim); }
    .top-bar strong { color: var(--cyan); }
    
    .dashboard-grid {
      display: grid;
      grid-template-columns: 2.3fr 1fr;
      gap: 14px;
      width: 100%;
    }
    @media (max-width: 768px) {
      .dashboard-grid { grid-template-columns: 1fr; }
    }
    .left-col {
      display: flex;
      flex-direction: column;
      gap: 14px;
    }
    .card {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 18px;
      position: relative;
    }

    /* BLOCK 1: Music Player */
    .music-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .speaker-pill {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      background: #1E293B;
      border: 1px solid var(--cyan);
      border-radius: 999px;
      padding: 3px 12px;
      font-size: 0.8rem;
      font-weight: 600;
      color: var(--cyan);
    }
    .music-status {
      font-size: 0.8rem;
      font-weight: 600;
      color: var(--emerald);
      display: flex;
      align-items: center;
      gap: 6px;
    }
    .track-title {
      font-size: 1.35rem;
      font-weight: 700;
      color: #FFF;
      margin-top: 14px;
      letter-spacing: -0.02em;
    }
    .track-artist {
      font-size: 0.95rem;
      color: var(--text-dim);
      margin-top: 2px;
      margin-bottom: 16px;
    }
    .music-controls {
      display: flex;
      align-items: center;
      gap: 12px;
    }
    .ctrl-btn {
      background: #1E293B;
      border: 1px solid #334155;
      color: #FFF;
      border-radius: 8px;
      padding: 8px 14px;
      font-size: 0.95rem;
      cursor: pointer;
      transition: all 0.15s;
    }
    .ctrl-btn:hover { background: #334155; }
    .ctrl-play {
      background: var(--accent);
      border-color: var(--accent-glow);
      padding: 8px 18px;
      box-shadow: 0 0 12px rgba(37, 99, 235, 0.4);
    }
    .progress-container {
      flex: 1;
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .progress-bar-bg {
      flex: 1;
      height: 6px;
      background: #334155;
      border-radius: 3px;
      overflow: hidden;
      cursor: pointer;
    }
    .progress-bar-fill {
      height: 100%;
      width: 25%;
      background: var(--cyan);
      border-radius: 3px;
      transition: width 0.3s;
    }
    .time-readout {
      font-size: 0.8rem;
      color: var(--text-muted);
      font-variant-numeric: tabular-nums;
    }

    /* BLOCK 2: Climate, Clock & Water Node */
    .clock-row {
      display: flex;
      align-items: baseline;
      gap: 10px;
      margin-bottom: 12px;
    }
    .clock-time {
      font-size: 2.4rem;
      font-weight: 800;
      letter-spacing: -0.03em;
    }
    .clock-ampm {
      font-size: 0.95rem;
      font-weight: 700;
      color: var(--cyan);
    }
    .clock-date {
      font-size: 0.95rem;
      color: var(--text-dim);
      margin-left: auto;
    }
    .divider {
      height: 1px;
      background: var(--border);
      margin-bottom: 16px;
    }
    .telemetry-grid {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 10px;
      margin-bottom: 16px;
    }
    @media (max-width: 600px) {
      .telemetry-grid { grid-template-columns: 1fr 1fr; }
    }
    .sub-card {
      background: var(--card-sub);
      border: 1px solid var(--border-sub);
      border-radius: 10px;
      padding: 12px;
    }
    .sub-label {
      font-size: 0.7rem;
      font-weight: 700;
      letter-spacing: 0.05em;
      text-transform: uppercase;
    }
    .sub-val {
      font-size: 1.35rem;
      font-weight: 700;
      margin-top: 4px;
      color: #FFF;
    }
    .sub-badge {
      font-size: 0.65rem;
      color: var(--text-muted);
      margin-top: 3px;
      display: block;
    }

    .valve-action-box {
      background: rgba(14, 20, 32, 0.7);
      border: 1px solid var(--border-sub);
      border-radius: 10px;
      padding: 12px 16px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 12px;
    }
    @media (max-width: 600px) {
      .valve-action-box { flex-direction: column; align-items: stretch; }
    }
    .valve-status-indicator {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .status-dot {
      width: 14px;
      height: 14px;
      border-radius: 50%;
      background: var(--crimson);
      box-shadow: 0 0 10px var(--crimson);
      transition: all 0.3s;
    }
    .valve-title {
      font-size: 1.1rem;
      font-weight: 700;
    }
    .valve-btn-group {
      display: flex;
      gap: 10px;
    }
    .action-btn {
      padding: 10px 18px;
      border-radius: 8px;
      border: none;
      font-weight: 700;
      font-size: 0.9rem;
      cursor: pointer;
      display: flex;
      align-items: center;
      gap: 6px;
      transition: transform 0.1s, opacity 0.2s;
    }
    .action-btn:active { transform: scale(0.96); }
    .btn-open { background: var(--emerald); color: white; box-shadow: 0 0 12px rgba(16, 185, 129, 0.3); }
    .btn-close { background: var(--crimson); color: white; box-shadow: 0 0 12px rgba(239, 68, 68, 0.3); }

    /* BLOCK 3: Rooms Column */
    .rooms-card {
      display: flex;
      flex-direction: column;
    }
    .rooms-header {
      font-size: 0.75rem;
      font-weight: 700;
      color: var(--text-muted);
      letter-spacing: 0.15em;
      text-align: center;
      margin-bottom: 14px;
    }
    .rooms-list {
      display: flex;
      flex-direction: column;
      gap: 10px;
      flex: 1;
    }
    .room-btn {
      background: #1E293B;
      border: 1px solid #334155;
      color: #CBD5E1;
      border-radius: 8px;
      padding: 14px 16px;
      font-size: 0.9rem;
      font-weight: 600;
      cursor: pointer;
      text-align: center;
      transition: all 0.2s;
    }
    .room-btn:hover { background: #2A384C; color: #FFF; }
    .room-btn.active {
      background: var(--accent);
      border-color: var(--accent-glow);
      color: #FFFFFF;
      box-shadow: 0 0 14px rgba(37, 99, 235, 0.4);
    }

    .alarm-pill {
      display: none;
      background: rgba(245, 158, 11, 0.2);
      border: 1px solid var(--amber);
      color: var(--amber);
      border-radius: 8px;
      padding: 8px;
      font-size: 0.8rem;
      font-weight: 700;
      text-align: center;
      margin-bottom: 12px;
    }
  </style>
</head>
<body>
  <div class="dashboard-wrapper">
    <div class="top-bar">
      <div>Project Lyra / Nova • <strong>ESP32-S3 Node</strong></div>
      <div id="net-badge"><span>IP:</span> <strong id="ip-val">Connecting...</strong> | <span>Signal:</span> <strong id="rssi-val">-</strong></div>
    </div>

    <div id="alarm-box" class="alarm-pill">⚠️ ALARM TRIGGERED</div>

    <div class="dashboard-grid">
      <!-- LEFT COLUMN -->
      <div class="left-col">
        <!-- BLOCK 1: Music Player -->
        <div class="card">
          <div class="music-header">
            <div class="speaker-pill">
              <span>🔊</span> <span id="active-speaker-name">Living Room</span>
            </div>
            <div id="music-status" class="music-status">
              <span>▶</span> <span id="status-text">Playing</span>
            </div>
          </div>

          <div id="track-title" class="track-title">Blinding Lights</div>
          <div id="track-artist" class="track-artist">The Weeknd</div>

          <div class="music-controls">
            <button class="ctrl-btn" onclick="prevTrack()">⏮</button>
            <button id="play-pause-btn" class="ctrl-btn ctrl-play" onclick="togglePlayPause()">⏸</button>
            <button class="ctrl-btn" onclick="nextTrack()">⏭</button>

            <div class="progress-container">
              <div class="progress-bar-bg" onclick="seekTrack(event)">
                <div id="progress-fill" class="progress-bar-fill"></div>
              </div>
              <div id="time-pos" class="time-readout">0:45</div>
            </div>
          </div>
        </div>

        <!-- BLOCK 2: Time, Climate & Water Node -->
        <div class="card">
          <div class="clock-row">
            <div id="clock-digits" class="clock-time">10:45</div>
            <div id="clock-ampm" class="clock-ampm">AM</div>
            <div id="clock-date" class="clock-date">Wed, 23 Sep</div>
          </div>

          <div class="divider"></div>

          <div class="telemetry-grid">
            <div class="sub-card">
              <div class="sub-label" style="color:var(--orange);">TEMP</div>
              <div class="sub-val" id="temp-val">24.5 °C</div>
              <span class="sub-badge">Indoor Sensor</span>
            </div>
            <div class="sub-card">
              <div class="sub-label" style="color:var(--cyan);">💧 HUMID</div>
              <div class="sub-val" id="humid-val">58 %</div>
              <span class="sub-badge">Relative</span>
            </div>
            <div class="sub-card">
              <div class="sub-label" style="color:var(--cyan);">🌊 FLOW RATE</div>
              <div class="sub-val" id="flow-rate-val">0.0 L/m</div>
              <span class="sub-badge" id="flow-diag">Pulses: 0 • Pin: 1</span>
            </div>
            <div class="sub-card">
              <div class="sub-label" style="color:var(--emerald);">💧 TOTAL VOL</div>
              <div class="sub-val" id="total-vol-val">0.00 L</div>
              <span class="sub-badge">NVS Persistent</span>
            </div>
          </div>

          <!-- Valve Control Section -->
          <div class="valve-action-box">
            <div class="valve-status-indicator">
              <div id="status-dot" class="status-dot"></div>
              <div>
                <div id="valve-label" class="valve-title" style="color:var(--crimson);">VALVE: CLOSED</div>
                <div id="flow-subtext" style="font-size:0.75rem; color:var(--text-muted);">Idle • No water movement</div>
              </div>
            </div>
            <div class="valve-btn-group">
              <button class="action-btn btn-open" onclick="sendValveCmd('open')">▶ OPEN TAP</button>
              <button class="action-btn btn-close" onclick="sendValveCmd('close')">■ CLOSE TAP</button>
            </div>
          </div>
        </div>
      </div>

      <!-- RIGHT COLUMN: BLOCK 3: Rooms Menu -->
      <div class="card rooms-card">
        <div class="rooms-header">ROOMS</div>
        <div class="rooms-list">
          <button class="room-btn active" onclick="selectRoom(0, 'Living Room')">Living Room</button>
          <button class="room-btn" onclick="selectRoom(1, 'Kitchen')">Kitchen</button>
          <button class="room-btn" onclick="selectRoom(2, 'Dining')">Dining</button>
          <button class="room-btn" onclick="selectRoom(3, 'Bedroom')">Bedroom</button>
          <button class="room-btn" onclick="selectRoom(4, 'Bathroom')">Bathroom</button>
        </div>
      </div>
    </div>
  </div>

  <script>
    const PLAYLIST = [
      { title: "Blinding Lights", artist: "The Weeknd", duration: 200 },
      { title: "As It Was", artist: "Harry Styles", duration: 167 },
      { title: "Levitating", artist: "Dua Lipa", duration: 203 },
      { title: "Shape of You", artist: "Ed Sheeran", duration: 233 }
    ];
    let currentTrackIdx = 0;
    let isPlaying = true;
    let elapsedSec = 45;

    function updateTrackUI() {
      const t = PLAYLIST[currentTrackIdx];
      document.getElementById('track-title').textContent = t.title;
      document.getElementById('track-artist').textContent = t.artist;
      const pct = (elapsedSec / t.duration) * 100;
      document.getElementById('progress-fill').style.width = pct + '%';
      const m = Math.floor(elapsedSec / 60);
      const s = String(elapsedSec % 60).padStart(2, '0');
      document.getElementById('time-pos').textContent = m + ':' + s;
    }

    function togglePlayPause() {
      isPlaying = !isPlaying;
      const btn = document.getElementById('play-pause-btn');
      const st = document.getElementById('music-status');
      if (isPlaying) {
        btn.textContent = '⏸';
        st.innerHTML = '<span>▶</span> Playing';
        st.style.color = 'var(--emerald)';
      } else {
        btn.textContent = '▶';
        st.innerHTML = '<span>⏸</span> Paused';
        st.style.color = 'var(--text-muted)';
      }
    }

    function nextTrack() {
      currentTrackIdx = (currentTrackIdx + 1) % PLAYLIST.length;
      elapsedSec = 0;
      updateTrackUI();
    }

    function prevTrack() {
      currentTrackIdx = (currentTrackIdx === 0) ? PLAYLIST.length - 1 : currentTrackIdx - 1;
      elapsedSec = 0;
      updateTrackUI();
    }

    // Music progress ticker
    setInterval(() => {
      if (isPlaying) {
        elapsedSec++;
        if (elapsedSec >= PLAYLIST[currentTrackIdx].duration) {
          nextTrack();
        } else {
          updateTrackUI();
        }
      }
    }, 1000);

    // Live Clock Ticker
    function updateClock() {
      const now = new Date();
      let hours = now.getHours();
      const ampm = hours >= 12 ? 'PM' : 'AM';
      hours = hours % 12 || 12;
      const mins = String(now.getMinutes()).padStart(2, '0');
      document.getElementById('clock-digits').textContent = `${hours}:${mins}`;
      document.getElementById('clock-ampm').textContent = ampm;
      const options = { weekday: 'short', day: 'numeric', month: 'short' };
      document.getElementById('clock-date').textContent = now.toLocaleDateString('en-US', options);
    }
    setInterval(updateClock, 1000);
    updateClock();

    // Rooms Selection
    function selectRoom(idx, name) {
      const btns = document.querySelectorAll('.room-btn');
      btns.forEach((b, i) => {
        b.className = (i === idx) ? 'room-btn active' : 'room-btn';
      });
      document.getElementById('active-speaker-name').textContent = name;
    }

    // Real-Time ESP32 Telemetry Polling
    async function fetchStatus() {
      try {
        const res = await fetch('/api/status');
        const data = await res.json();

        // Valve & Water
        const isOpen = data.valve.state === 'OPEN';
        const isFlowing = data.telemetry.is_flowing;
        const vLabel = document.getElementById('valve-label');
        const dot = document.getElementById('status-dot');
        const sub = document.getElementById('flow-subtext');

        if (isOpen) {
          vLabel.textContent = 'VALVE: OPEN';
          vLabel.style.color = 'var(--emerald)';
          dot.style.background = 'var(--emerald)';
          dot.style.boxShadow = '0 0 10px var(--emerald)';
        } else {
          vLabel.textContent = 'VALVE: CLOSED';
          vLabel.style.color = 'var(--crimson)';
          dot.style.background = 'var(--crimson)';
          dot.style.boxShadow = '0 0 10px var(--crimson)';
        }

        sub.textContent = isFlowing ? 'Water actively flowing!' : 'Idle • No water movement';
        document.getElementById('flow-rate-val').textContent = data.telemetry.flow_rate_lpm.toFixed(1) + ' L/m';
        document.getElementById('total-vol-val').textContent = data.telemetry.total_volume_l.toFixed(2) + ' L';

        // Diagnostics
        const pulses = data.telemetry.raw_pulses !== undefined ? data.telemetry.raw_pulses : 0;
        const pin = data.telemetry.pin_level !== undefined ? data.telemetry.pin_level : '-';
        document.getElementById('flow-diag').textContent = `Pulses: ${pulses} • Pin: ${pin}`;

        // Alarms
        const alarmBox = document.getElementById('alarm-box');
        if (data.alarm && data.alarm.code !== 0) {
          alarmBox.style.display = 'block';
          alarmBox.textContent = '⚠️ ALARM: ' + data.alarm.status;
        } else {
          alarmBox.style.display = 'none';
        }

        // Network
        document.getElementById('ip-val').textContent = data.system.ip;
        document.getElementById('rssi-val').textContent = data.system.wifi_rssi + ' dBm';
      } catch (e) {
        console.warn('Status poll error:', e);
      }
    }

    async function sendValveCmd(cmd) {
      await fetch('/api/valve?state=' + cmd, { method: 'POST' });
      setTimeout(fetchStatus, 200);
    }

    setInterval(fetchStatus, 1000);
    fetchStatus();
    updateTrackUI();
  </script>
</body>
</html>
)rawliteral";


NetworkManager::NetworkManager()
    : _server(80),
      _cmd_queue(nullptr),
      _tel_mutex(nullptr),
      _shared_tel(nullptr),
      _wifi_connected(false),
      _is_soft_ap(false),
      _last_wifi_check_ms(0),
      _ip_address("0.0.0.0"),
      _hostname(DEVICE_HOSTNAME) {}

void NetworkManager::begin(QueueHandle_t cmd_queue, SemaphoreHandle_t tel_mutex, const SystemTelemetry* shared_tel) {
    _cmd_queue = cmd_queue;
    _tel_mutex = tel_mutex;
    _shared_tel = shared_tel;

    Serial.println("\n[NET] Initializing Network Engine...");

    // Connect to WiFi or fallback to SoftAP
    connectWiFi();

    // Start mDNS Responder
    if (MDNS.begin(_hostname.c_str())) {
        Serial.printf("[NET] mDNS responder started: http://%s.local\n", _hostname.c_str());
        MDNS.addService("http", "tcp", 80);
        MDNS.addServiceTxt("http", "tcp", "device", DEVICE_FRIENDLY_NAME);
        MDNS.addServiceTxt("http", "tcp", "room", DEVICE_ROOM);
    } else {
        Serial.println("[NET] Warning: Failed to initialize mDNS");
    }

    // Configure REST routes and start HTTP server
    setupRoutes();
    _server.begin();
    Serial.printf("[NET] HTTP REST Server listening on port 80 at %s\n", _ip_address.c_str());
}

void NetworkManager::connectWiFi() {
    String ssid = DEFAULT_WIFI_SSID;
    String pass = DEFAULT_WIFI_PASS;

    // Check if real credentials are provided
    if (ssid == "WIFI_SSID_HERE" || ssid.length() == 0) {
        Serial.println("[NET] No default WiFi credentials set in config.h. Starting SoftAP mode...");
        startSoftAP();
        return;
    }

    Serial.printf("[NET] Connecting to WiFi SSID: %s ...\n", ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    uint32_t start_ms = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start_ms < WIFI_CONNECT_TIMEOUT_MS)) {
        delay(250);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        _wifi_connected = true;
        _is_soft_ap = false;
        _ip_address = WiFi.localIP().toString();
        Serial.printf("[NET] WiFi Connected! IP Address: %s (Signal: %d dBm)\n",
                      _ip_address.c_str(), WiFi.RSSI());
    } else {
        Serial.println("[NET] WiFi connection timed out. Falling back to SoftAP...");
        startSoftAP();
    }
}

void NetworkManager::startSoftAP() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char ap_name[32];
    snprintf(ap_name, sizeof(ap_name), "%s%02X%02X", AP_SSID_PREFIX, mac[4], mac[5]);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_name, AP_DEFAULT_PASS);
    _wifi_connected = false;
    _is_soft_ap = true;
    _ip_address = WiFi.softAPIP().toString();

    Serial.printf("[NET] SoftAP Started! SSID: '%s' | Password: '%s'\n", ap_name, AP_DEFAULT_PASS);
    Serial.printf("[NET] Access the node at http://%s\n", _ip_address.c_str());
}

void NetworkManager::sendCorsHeaders() {
    _server.sendHeader("Access-Control-Allow-Origin", "*");
    _server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    _server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void NetworkManager::setupRoutes() {
    // Root: Embedded HTML Web Dashboard
    _server.on("/", HTTP_GET, [this]() {
        handleRoot();
    });

    // API Ping
    _server.on("/api/ping", HTTP_GET, [this]() {
        handlePing();
    });

    // API Telemetry Status
    _server.on("/api/status", HTTP_GET, [this]() {
        handleStatus();
    });

    // API Valve Control (Supports GET for quick URL triggers & POST for REST apps)
    _server.on("/api/valve", HTTP_GET, [this]() {
        handleValve();
    });
    _server.on("/api/valve", HTTP_POST, [this]() {
        handleValve();
    });

    // API Alarms & Reset
    _server.on("/api/alarm/reset", HTTP_POST, [this]() {
        handleAlarmReset();
    });
    _server.on("/api/volume/reset", HTTP_POST, [this]() {
        handleVolumeReset();
    });

    // API Device Info & Capabilities
    _server.on("/api/device", HTTP_GET, [this]() {
        handleDevice();
    });

    // Pre-flight CORS Options
    _server.onNotFound([this]() {
        if (_server.method() == HTTP_OPTIONS) {
            sendCorsHeaders();
            _server.send(204);
        } else {
            handleNotFound();
        }
    });
}

void NetworkManager::handleRoot() {
    _server.send_P(200, "text/html", HTML_DASHBOARD);
}

void NetworkManager::handlePing() {
    sendCorsHeaders();
    StaticJsonDocument<128> doc;
    doc["status"] = "ok";
    doc["device"] = DEVICE_FRIENDLY_NAME;
    doc["uptime_s"] = millis() / 1000;

    String resp;
    serializeJson(doc, resp);
    _server.send(200, "application/json", resp);
}

void NetworkManager::handleStatus() {
    sendCorsHeaders();

    SystemTelemetry tel;
    if (_tel_mutex && xSemaphoreTake(_tel_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        tel = *_shared_tel;
        xSemaphoreGive(_tel_mutex);
    }

    StaticJsonDocument<512> doc;
    doc["device_id"] = "esp32_kitchen_valve";
    doc["name"] = DEVICE_FRIENDLY_NAME;
    doc["room"] = DEVICE_ROOM;

    // Valve section
    JsonObject v = doc.createNestedObject("valve");
    v["is_open"] = (tel.valve_state == ValveState::OPEN);
    switch (tel.valve_state) {
        case ValveState::OPEN: v["state"] = "OPEN"; break;
        case ValveState::CLOSED: v["state"] = "CLOSED"; break;
        case ValveState::MOVING_TO_OPEN: v["state"] = "OPENING"; break;
        case ValveState::MOVING_TO_CLOSE: v["state"] = "CLOSING"; break;
    }

    // Telemetry section
    JsonObject t = doc.createNestedObject("telemetry");
    t["flow_rate_lpm"] = ((int)(tel.flow_rate_lpm * 100)) / 100.0f;
    t["session_volume_l"] = ((int)(tel.session_volume_l * 100)) / 100.0f;
    t["total_volume_l"] = ((int)(tel.total_volume_l * 100)) / 100.0f;
    t["session_duration_s"] = tel.session_duration_s;
    t["is_flowing"] = tel.is_flowing;
    t["raw_pulses"] = tel.raw_pulse_count;
    t["pin_level"] = tel.pin_level;


    // Alarm section
    JsonObject a = doc.createNestedObject("alarm");
    a["code"] = (uint8_t)tel.safety_alarm;
    switch (tel.safety_alarm) {
        case SafetyAlarm::NONE: a["status"] = "NONE"; break;
        case SafetyAlarm::LEAK_DETECTED: a["status"] = "LEAK_DETECTED"; break;
        case SafetyAlarm::MAX_TIME_EXCEEDED: a["status"] = "MAX_TIME_EXCEEDED"; break;
        case SafetyAlarm::MAX_VOLUME_EXCEEDED: a["status"] = "MAX_VOLUME_EXCEEDED"; break;
        default: a["status"] = "FAULT"; break;
    }

    // System section
    JsonObject s = doc.createNestedObject("system");
    s["uptime_s"] = tel.uptime_s;
    s["free_heap"] = tel.free_heap;
    s["free_psram"] = tel.free_psram;
    s["ip"] = _ip_address;
    s["wifi_connected"] = _wifi_connected;
    s["wifi_rssi"] = _wifi_connected ? WiFi.RSSI() : 0;

    String resp;
    serializeJson(doc, resp);
    _server.send(200, "application/json", resp);
}

void NetworkManager::handleValve() {
    sendCorsHeaders();

    String state_param = "";
    if (_server.hasArg("state")) {
        state_param = _server.arg("state");
    } else if (_server.hasArg("action")) {
        state_param = _server.arg("action");
    } else if (_server.hasArg("plain")) {
        StaticJsonDocument<128> body;
        DeserializationError err = deserializeJson(body, _server.arg("plain"));
        if (!err) {
            if (body.containsKey("state")) state_param = body["state"].as<String>();
            else if (body.containsKey("action")) state_param = body["action"].as<String>();
        }
    }

    state_param.toLowerCase();
    ControlCommand cmd;
    bool valid = true;

    if (state_param == "open" || state_param == "1" || state_param == "true") {
        cmd.type = ControlCommandType::CMD_VALVE_OPEN;
        Serial.println("[NET] REST API: CMD_VALVE_OPEN");
    } else if (state_param == "close" || state_param == "0" || state_param == "false") {
        cmd.type = ControlCommandType::CMD_VALVE_CLOSE;
        Serial.println("[NET] REST API: CMD_VALVE_CLOSE");
    } else if (state_param == "toggle") {
        cmd.type = ControlCommandType::CMD_VALVE_TOGGLE;
        Serial.println("[NET] REST API: CMD_VALVE_TOGGLE");
    } else {
        valid = false;
    }

    StaticJsonDocument<128> resp_doc;
    if (valid && _cmd_queue) {
        xQueueSend(_cmd_queue, &cmd, 0);
        resp_doc["success"] = true;
        resp_doc["command"] = state_param;
        resp_doc["message"] = "Valve command dispatched";
    } else {
        resp_doc["success"] = false;
        resp_doc["error"] = "Invalid command. Use state=open, close, or toggle";
    }

    String resp;
    serializeJson(resp_doc, resp);
    _server.send(valid ? 200 : 400, "application/json", resp);
}

void NetworkManager::handleAlarmReset() {
    sendCorsHeaders();
    ControlCommand cmd;
    cmd.type = ControlCommandType::CMD_RESET_ALARM;
    if (_cmd_queue) {
        xQueueSend(_cmd_queue, &cmd, 0);
    }

    StaticJsonDocument<128> doc;
    doc["success"] = true;
    doc["message"] = "Alarm reset dispatched";

    String resp;
    serializeJson(doc, resp);
    _server.send(200, "application/json", resp);
}

void NetworkManager::handleVolumeReset() {
    sendCorsHeaders();
    ControlCommand cmd;
    cmd.type = ControlCommandType::CMD_RESET_VOLUME;
    if (_cmd_queue) {
        xQueueSend(_cmd_queue, &cmd, 0);
    }

    StaticJsonDocument<128> doc;
    doc["success"] = true;
    doc["message"] = "Volume reset dispatched";

    String resp;
    serializeJson(doc, resp);
    _server.send(200, "application/json", resp);
}

void NetworkManager::handleDevice() {
    sendCorsHeaders();
    StaticJsonDocument<384> doc;
    doc["device_id"] = "esp32_kitchen_valve";
    doc["name"] = DEVICE_FRIENDLY_NAME;
    doc["room"] = DEVICE_ROOM;
    doc["hostname"] = _hostname;
    doc["mac"] = WiFi.macAddress();
    doc["ip"] = _ip_address;

    JsonArray caps = doc.createNestedArray("capabilities");
    caps.add("valve");
    caps.add("flow_sensor");
    caps.add("safety_alarm");

    JsonObject endpoints = doc.createNestedObject("endpoints");
    endpoints["status"] = "/api/status";
    endpoints["valve"] = "/api/valve";
    endpoints["alarm_reset"] = "/api/alarm/reset";
    endpoints["volume_reset"] = "/api/volume/reset";

    String resp;
    serializeJson(doc, resp);
    _server.send(200, "application/json", resp);
}

void NetworkManager::handleNotFound() {
    sendCorsHeaders();
    StaticJsonDocument<128> doc;
    doc["error"] = "Resource not found";
    doc["endpoints"] = "/api/status, /api/valve, /api/device, /";

    String resp;
    serializeJson(doc, resp);
    _server.send(404, "application/json", resp);
}

void NetworkManager::update() {
    _server.handleClient();

    // Check WiFi health every 10 seconds if not in SoftAP mode
    uint32_t now = millis();
    if (!_is_soft_ap && (now - _last_wifi_check_ms >= 10000)) {
        _last_wifi_check_ms = now;
        if (WiFi.status() != WL_CONNECTED && _wifi_connected) {
            _wifi_connected = false;
            Serial.println("[NET] WiFi connection lost! Reconnecting...");
            WiFi.reconnect();
        }
    }
}

bool NetworkManager::isConnected() const {
    return _wifi_connected;
}

bool NetworkManager::isSoftAP() const {
    return _is_soft_ap;
}

String NetworkManager::getIP() const {
    return _ip_address;
}

String NetworkManager::getSSID() const {
    return _is_soft_ap ? "SoftAP" : WiFi.SSID();
}

String NetworkManager::getHostname() const {
    return _hostname;
}

int8_t NetworkManager::getRSSI() const {
    return _wifi_connected ? WiFi.RSSI() : 0;
}
