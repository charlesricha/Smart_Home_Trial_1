/**
 * @file network_manager.h
 * @brief ESP32 Local Networking, mDNS, and REST API Engine (Milestone 1)
 * 
 * Provides:
 * - WiFi Station connection with automatic SoftAP fallback
 * - mDNS responder (http://smarthome-kitchen.local)
 * - REST API endpoints:
 *     - GET  /api/ping
 *     - GET  /api/status
 *     - GET/POST /api/valve?state=open|close|toggle
 *     - POST /api/alarm/reset
 *     - POST /api/volume/reset
 *     - GET  /api/device
 * - Embedded responsive dark-theme Web Dashboard at GET /
 */

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "types.h"
#include "config.h"

class NetworkManager {
public:
    NetworkManager();

    void begin(QueueHandle_t cmd_queue, SemaphoreHandle_t tel_mutex, const SystemTelemetry* shared_tel);
    void update();

    bool isConnected() const;
    bool isSoftAP() const;
    String getIP() const;
    String getSSID() const;
    String getHostname() const;
    int8_t getRSSI() const;

private:
    void setupRoutes();
    void connectWiFi();
    void startSoftAP();
    void sendCorsHeaders();

    // REST & Web Handlers
    void handleRoot();
    void handlePing();
    void handleStatus();
    void handleValve();
    void handleAlarmReset();
    void handleVolumeReset();
    void handleDevice();
    void handleNotFound();

    WebServer _server;
    QueueHandle_t _cmd_queue;
    SemaphoreHandle_t _tel_mutex;
    const SystemTelemetry* _shared_tel;

    bool _wifi_connected;
    bool _is_soft_ap;
    uint32_t _last_wifi_check_ms;
    String _ip_address;
    String _hostname;
};

extern NetworkManager networkManager;
