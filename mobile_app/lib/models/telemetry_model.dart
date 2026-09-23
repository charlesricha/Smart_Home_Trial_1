/**
 * @file telemetry_model.dart
 * @brief Data model for ESP32 Smart Home Node telemetry
 */

class NodeTelemetry {
  final String deviceId;
  final String deviceName;
  final String room;
  final String valveState;
  final bool isValveOpen;
  final double flowRateLpm;
  final double sessionVolL;
  final double totalVolL;
  final int sessionDurationS;
  final bool isFlowing;
  final int rawPulses;
  final int pinLevel;
  final int alarmCode;
  final String alarmStatus;
  final int uptimeS;
  final String ipAddress;
  final int wifiRssi;

  NodeTelemetry({
    required this.deviceId,
    required this.deviceName,
    required this.room,
    required this.valveState,
    required this.isValveOpen,
    required this.flowRateLpm,
    required this.sessionVolL,
    required this.totalVolL,
    required this.sessionDurationS,
    required this.isFlowing,
    required this.rawPulses,
    required this.pinLevel,
    required this.alarmCode,
    required this.alarmStatus,
    required this.uptimeS,
    required this.ipAddress,
    required this.wifiRssi,
  });

  factory NodeTelemetry.fromJson(Map<String, dynamic> json) {
    final valve = json['valve'] as Map<String, dynamic>? ?? {};
    final tel = json['telemetry'] as Map<String, dynamic>? ?? {};
    final alarm = json['alarm'] as Map<String, dynamic>? ?? {};
    final sys = json['system'] as Map<String, dynamic>? ?? {};

    return NodeTelemetry(
      deviceId: json['device_id'] ?? 'unknown',
      deviceName: json['name'] ?? 'Smart Home Node',
      room: json['room'] ?? 'Kitchen',
      valveState: valve['state'] ?? 'CLOSED',
      isValveOpen: valve['is_open'] ?? false,
      flowRateLpm: (tel['flow_rate_lpm'] as num?)?.toDouble() ?? 0.0,
      sessionVolL: (tel['session_volume_l'] as num?)?.toDouble() ?? 0.0,
      totalVolL: (tel['total_volume_l'] as num?)?.toDouble() ?? 0.0,
      sessionDurationS: (tel['session_duration_s'] as num?)?.toInt() ?? 0,
      isFlowing: tel['is_flowing'] ?? false,
      rawPulses: (tel['raw_pulses'] as num?)?.toInt() ?? 0,
      pinLevel: (tel['pin_level'] as num?)?.toInt() ?? 1,
      alarmCode: (alarm['code'] as num?)?.toInt() ?? 0,
      alarmStatus: alarm['status'] ?? 'NONE',
      uptimeS: (sys['uptime_s'] as num?)?.toInt() ?? 0,
      ipAddress: sys['ip'] ?? '0.0.0.0',
      wifiRssi: (sys['wifi_rssi'] as num?)?.toInt() ?? 0,
    );
  }

  factory NodeTelemetry.empty() {
    return NodeTelemetry(
      deviceId: 'connecting',
      deviceName: 'Connecting...',
      room: 'Kitchen',
      valveState: 'CLOSED',
      isValveOpen: false,
      flowRateLpm: 0.0,
      sessionVolL: 0.0,
      totalVolL: 0.0,
      sessionDurationS: 0,
      isFlowing: false,
      rawPulses: 0,
      pinLevel: 1,
      alarmCode: 0,
      alarmStatus: 'NONE',
      uptimeS: 0,
      ipAddress: 'Connecting...',
      wifiRssi: 0,
    );
  }
}
