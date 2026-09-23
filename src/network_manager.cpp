/**
 * @file network_manager.cpp
 * @brief ESP32 Local Networking, mDNS, and REST API Implementation
 */

#include "network_manager.h"

NetworkManager networkManager;

// Embedded Responsive Dark Theme Dashboard
static const char HTML_DASHBOARD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Home - Water Controller</title>
  <style>
    :root {
      --bg: #0B0F19;
      --card: #141C2B;
      --border: #232F42;
      --accent: #2563EB;
      --accent-glow: #38BDF8;
      --text: #F8FAFC;
      --text-dim: #94A3B8;
      --emerald: #10B981;
      --crimson: #EF4444;
      --amber: #F59E0B;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
    body { background: var(--bg); color: var(--text); padding: 16px; display: flex; justify-content: center; }
    .container { width: 100%; max-width: 480px; display: flex; flex-direction: column; gap: 14px; }
    .header { display: flex; justify-content: space-between; align-items: center; padding: 12px 16px; background: var(--card); border: 1px solid var(--border); border-radius: 12px; }
    .header h1 { font-size: 1.1rem; font-weight: 600; }
    .badge { font-size: 0.75rem; padding: 4px 8px; border-radius: 999px; background: #1E293B; border: 1px solid var(--accent-glow); color: var(--accent-glow); font-weight: 500; }
    .card { background: var(--card); border: 1px solid var(--border); border-radius: 12px; padding: 16px; display: flex; flex-direction: column; gap: 12px; }
    .valve-hero { display: flex; flex-direction: column; align-items: center; justify-content: center; padding: 20px; text-align: center; }
    .valve-icon { width: 72px; height: 72px; border-radius: 50%; display: flex; align-items: center; justify-content: center; font-size: 2rem; margin-bottom: 12px; border: 2px solid var(--border); transition: all 0.3s; }
    .valve-open { background: rgba(16, 185, 129, 0.15); border-color: var(--emerald); color: var(--emerald); box-shadow: 0 0 20px rgba(16, 185, 129, 0.3); }
    .valve-closed { background: rgba(239, 68, 68, 0.15); border-color: var(--crimson); color: var(--crimson); box-shadow: 0 0 20px rgba(239, 68, 68, 0.2); }
    .status-text { font-size: 1.4rem; font-weight: 700; letter-spacing: 0.05em; }
    .btn-group { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-top: 8px; }
    button { padding: 14px; border: none; border-radius: 10px; font-size: 1rem; font-weight: 600; cursor: pointer; transition: transform 0.1s, opacity 0.2s; display: flex; align-items: center; justify-content: center; gap: 8px; }
    button:active { transform: scale(0.97); }
    .btn-open { background: var(--emerald); color: white; }
    .btn-close { background: var(--crimson); color: white; }
    .btn-reset { background: #334155; color: var(--text); padding: 10px; font-size: 0.85rem; border: 1px solid var(--border); }
    .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
    .stat-box { background: #0E1420; border: 1px solid #1F2B3E; border-radius: 8px; padding: 12px; }
    .stat-label { font-size: 0.75rem; color: var(--text-dim); text-transform: uppercase; font-weight: 600; }
    .stat-val { font-size: 1.3rem; font-weight: 700; color: #FFF; margin-top: 4px; }
    .alarm-banner { display: none; background: rgba(245, 158, 11, 0.2); border: 1px solid var(--amber); color: var(--amber); border-radius: 8px; padding: 10px; font-size: 0.85rem; text-align: center; font-weight: 600; }
    .footer { font-size: 0.75rem; color: var(--text-dim); text-align: center; margin-top: 8px; }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <div>
        <h1 id="device-title">Smart Home Node</h1>
        <div style="font-size:0.75rem; color:var(--text-dim);" id="network-info">Connecting...</div>
      </div>
      <div class="badge" id="room-tag">Kitchen</div>
    </div>

    <div id="alarm-box" class="alarm-banner">⚠️ SAFETY ALARM ACTIVE</div>

    <div class="card valve-hero">
      <div id="valve-icon" class="valve-icon valve-closed">💧</div>
      <div id="valve-status" class="status-text" style="color:var(--crimson);">VALVE CLOSED</div>
      <div style="font-size:0.85rem; color:var(--text-dim); margin-top:4px;" id="flow-indicator">Idle</div>

      <div class="btn-group" style="width:100%;">
        <button class="btn-open" onclick="sendValveCmd('open')">▶ OPEN TAP</button>
        <button class="btn-close" onclick="sendValveCmd('close')">■ CLOSE TAP</button>
      </div>
    </div>

    <div class="card">
      <div class="stat-label">Live Flow Telemetry</div>
      <div class="grid-2">
        <div class="stat-box">
          <div class="stat-label">Flow Rate</div>
          <div class="stat-val" id="flow-rate">0.0 L/m</div>
        </div>
        <div class="stat-box">
          <div class="stat-label">Session Vol</div>
          <div class="stat-val" id="session-vol">0.00 L</div>
        </div>
      </div>
      <div class="grid-2">
        <div class="stat-box">
          <div class="stat-label">Total Lifetime</div>
          <div class="stat-val" id="total-vol">0.00 L</div>
        </div>
        <div class="stat-box">
          <div class="stat-label">Uptime</div>
          <div class="stat-val" id="uptime">0s</div>
        </div>
      </div>
      <div style="display:flex; gap:8px; margin-top:6px;">
        <button class="btn-reset" style="flex:1;" onclick="resetAlarm()">Reset Alarm</button>
        <button class="btn-reset" style="flex:1;" onclick="resetVolume()">Reset Volume</button>
      </div>
    </div>

    <div class="footer">
      API: <code>POST /api/valve</code> | mDNS: <code>smarthome-kitchen.local</code>
    </div>
  </div>

  <script>
    async function updateData() {
      try {
        const res = await fetch('/api/status');
        const data = await res.json();
        
        const isClosed = data.valve.state === 'CLOSED';
        const isFlowing = data.telemetry.is_flowing;
        
        document.getElementById('valve-status').textContent = 'VALVE: ' + data.valve.state;
        document.getElementById('valve-status').style.color = isClosed ? 'var(--crimson)' : 'var(--emerald)';
        
        const icon = document.getElementById('valve-icon');
        icon.className = 'valve-icon ' + (isClosed ? 'valve-closed' : 'valve-open');
        icon.textContent = isFlowing ? '🌊' : '💧';
        
        document.getElementById('flow-indicator').textContent = isFlowing ? 'Water actively flowing...' : 'No flow detected';
        document.getElementById('flow-rate').textContent = data.telemetry.flow_rate_lpm.toFixed(1) + ' L/m';
        document.getElementById('session-vol').textContent = data.telemetry.session_volume_l.toFixed(2) + ' L';
        document.getElementById('total-vol').textContent = data.telemetry.total_volume_l.toFixed(2) + ' L';
        document.getElementById('uptime').textContent = data.system.uptime_s + 's';
        
        const alarmBox = document.getElementById('alarm-box');
        if (data.alarm.code !== 0) {
          alarmBox.style.display = 'block';
          alarmBox.textContent = '⚠️ ALARM: ' + data.alarm.status;
        } else {
          alarmBox.style.display = 'none';
        }
        
        document.getElementById('network-info').textContent = 'IP: ' + data.system.ip + ' | RSSI: ' + data.system.wifi_rssi + ' dBm';
      } catch (e) {
        console.error('Fetch error:', e);
      }
    }

    async function sendValveCmd(action) {
      await fetch('/api/valve?state=' + action, { method: 'POST' });
      setTimeout(updateData, 200);
    }
    
    async function resetAlarm() {
      await fetch('/api/alarm/reset', { method: 'POST' });
      setTimeout(updateData, 200);
    }

    async function resetVolume() {
      await fetch('/api/volume/reset', { method: 'POST' });
      setTimeout(updateData, 200);
    }

    setInterval(updateData, 1000);
    updateData();
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
