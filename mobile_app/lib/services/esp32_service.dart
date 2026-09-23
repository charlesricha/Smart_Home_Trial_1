/**
 * @file esp32_service.dart
 * @brief REST Client and live polling service for the ESP32 node
 */

import 'dart:async';
import 'dart:convert';
import 'package:flutter/foundation.dart';
import 'package:http/http.dart' as http;
import '../models/telemetry_model.dart';

class Esp32Service extends ChangeNotifier {
  String _baseUrl = 'http://smarthome-kitchen.local';
  NodeTelemetry _telemetry = NodeTelemetry.empty();
  bool _isConnected = false;
  Timer? _pollTimer;

  String get baseUrl => _baseUrl;
  NodeTelemetry get telemetry => _telemetry;
  bool get isConnected => _isConnected;

  Esp32Service() {
    startPolling();
  }

  void setBaseUrl(String newUrl) {
    if (!newUrl.startsWith('http://') && !newUrl.startsWith('https://')) {
      newUrl = 'http://$newUrl';
    }
    _baseUrl = newUrl.endsWith('/') ? newUrl.substring(0, newUrl.length - 1) : newUrl;
    fetchStatus();
    notifyListeners();
  }

  void startPolling() {
    _pollTimer?.cancel();
    fetchStatus();
    _pollTimer = Timer.periodic(const Duration(seconds: 1), (timer) {
      fetchStatus();
    });
  }

  Future<void> fetchStatus() async {
    try {
      final res = await http.get(Uri.parse('$_baseUrl/api/status')).timeout(
            const Duration(milliseconds: 1500),
          );

      if (res.statusCode == 200) {
        final data = json.decode(res.body);
        _telemetry = NodeTelemetry.fromJson(data);
        _isConnected = true;
      } else {
        _isConnected = false;
      }
    } catch (e) {
      _isConnected = false;
    }
    notifyListeners();
  }

  Future<bool> sendValveCommand(String action) async {
    try {
      final res = await http.post(
        Uri.parse('$_baseUrl/api/valve?state=$action'),
      ).timeout(const Duration(milliseconds: 2000));

      if (res.statusCode == 200) {
        // Immediately fetch updated state
        fetchStatus();
        return true;
      }
    } catch (e) {
      debugPrint('[ESP32] Error sending valve command: $e');
    }
    return false;
  }

  Future<bool> resetAlarm() async {
    try {
      final res = await http.post(Uri.parse('$_baseUrl/api/alarm/reset'))
          .timeout(const Duration(milliseconds: 2000));
      if (res.statusCode == 200) {
        fetchStatus();
        return true;
      }
    } catch (e) {
      debugPrint('[ESP32] Error resetting alarm: $e');
    }
    return false;
  }

  Future<bool> resetVolume() async {
    try {
      final res = await http.post(Uri.parse('$_baseUrl/api/volume/reset'))
          .timeout(const Duration(milliseconds: 2000));
      if (res.statusCode == 200) {
        fetchStatus();
        return true;
      }
    } catch (e) {
      debugPrint('[ESP32] Error resetting volume: $e');
    }
    return false;
  }

  @override
  void dispose() {
    _pollTimer?.cancel();
    super.dispose();
  }
}
