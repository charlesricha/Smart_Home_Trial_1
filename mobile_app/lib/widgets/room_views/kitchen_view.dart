/**
 * @file kitchen_view.dart
 * @brief Dynamic Room View: Kitchen (Controls water valve, flow meter, lights & safety)
 */

import 'package:flutter/material.dart';
import '../../models/telemetry_model.dart';
import '../../services/esp32_service.dart';

class KitchenView extends StatelessWidget {
  final NodeTelemetry telemetry;
  final Esp32Service service;

  const KitchenView({
    Key? key,
    required this.telemetry,
    required this.service,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    final isOpen = telemetry.isValveOpen;
    final isFlowing = telemetry.isFlowing;

    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        // Alarm Banner if active
        if (telemetry.alarmCode != 0)
          Container(
            width: double.infinity,
            margin: const EdgeInsets.only(bottom: 12),
            padding: const EdgeInsets.all(12),
            decoration: BoxDecoration(
              color: const Color(0xFFF59E0B).withOpacity(0.15),
              border: Border.all(color: const Color(0xFFF59E0B)),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Row(
              children: [
                const Icon(Icons.warning_amber_rounded, color: Color(0xFFF59E0B), size: 20),
                const SizedBox(width: 8),
                Expanded(
                  child: Text(
                    'SAFETY ALERT: ${telemetry.alarmStatus}',
                    style: const TextStyle(
                      color: Color(0xFFF59E0B),
                      fontWeight: FontWeight.bold,
                      fontSize: 12,
                    ),
                  ),
                ),
                TextButton(
                  onPressed: () => service.resetAlarm(),
                  child: const Text('RESET', style: TextStyle(color: Color(0xFFF59E0B))),
                ),
              ],
            ),
          ),

        // Hero Card: Water Valve Control
        Container(
          decoration: BoxDecoration(
            color: const Color(0xFF141C2B),
            borderRadius: BorderRadius.circular(14),
            border: Border.all(color: const Color(0xFF232F42)),
          ),
          padding: const EdgeInsets.all(16),
          child: Column(
            children: [
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Row(
                    children: [
                      Container(
                        width: 14,
                        height: 14,
                        decoration: BoxDecoration(
                          shape: BoxShape.circle,
                          color: isOpen ? const Color(0xFF10B981) : const Color(0xFFEF4444),
                          boxShadow: [
                            BoxShadow(
                              color: (isOpen ? const Color(0xFF10B981) : const Color(0xFFEF4444)).withOpacity(0.5),
                              blurRadius: 10,
                            ),
                          ],
                        ),
                      ),
                      const SizedBox(width: 10),
                      Text(
                        'TAP: ${telemetry.valveState}',
                        style: TextStyle(
                          fontSize: 16,
                          fontWeight: FontWeight.bold,
                          color: isOpen ? const Color(0xFF10B981) : const Color(0xFFEF4444),
                        ),
                      ),
                    ],
                  ),
                  Container(
                    padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                    decoration: BoxDecoration(
                      color: const Color(0xFF0E1420),
                      borderRadius: BorderRadius.circular(6),
                      border: Border.all(color: const Color(0xFF1F2B3E)),
                    ),
                    child: Text(
                      isFlowing ? '🌊 Flowing' : '💧 Idle',
                      style: TextStyle(
                        fontSize: 11,
                        color: isFlowing ? const Color(0xFF38BDF8) : const Color(0xFF64748B),
                        fontWeight: FontWeight.w600,
                      ),
                    ),
                  ),
                ],
              ),

              const SizedBox(height: 16),

              // Open / Close Action Buttons
              Row(
                children: [
                  Expanded(
                    child: ElevatedButton.icon(
                      style: ElevatedButton.styleFrom(
                        backgroundColor: const Color(0xFF10B981),
                        foregroundColor: Colors.white,
                        padding: const EdgeInsets.symmetric(vertical: 12),
                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
                        elevation: 0,
                      ),
                      icon: const Icon(Icons.play_arrow_rounded, size: 18),
                      label: const Text('OPEN TAP', style: TextStyle(fontWeight: FontWeight.bold)),
                      onPressed: () => service.sendValveCommand('open'),
                    ),
                  ),
                  const SizedBox(width: 10),
                  Expanded(
                    child: ElevatedButton.icon(
                      style: ElevatedButton.styleFrom(
                        backgroundColor: const Color(0xFFEF4444),
                        foregroundColor: Colors.white,
                        padding: const EdgeInsets.symmetric(vertical: 12),
                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
                        elevation: 0,
                      ),
                      icon: const Icon(Icons.stop_rounded, size: 18),
                      label: const Text('CLOSE TAP', style: TextStyle(fontWeight: FontWeight.bold)),
                      onPressed: () => service.sendValveCommand('close'),
                    ),
                  ),
                ],
              ),
            ],
          ),
        ),

        const SizedBox(height: 12),

        // Live Telemetry 3-Grid
        Row(
          children: [
            Expanded(
              child: _buildMetricCard(
                label: 'FLOW RATE',
                value: '${telemetry.flowRateLpm.toStringAsFixed(1)} L/m',
                sublabel: 'Pulses: ${telemetry.rawPulses}',
                color: const Color(0xFF38BDF8),
              ),
            ),
            const SizedBox(width: 10),
            Expanded(
              child: _buildMetricCard(
                label: 'SESSION VOL',
                value: '${telemetry.sessionVolL.toStringAsFixed(2)} L',
                sublabel: '${telemetry.sessionDurationS}s active',
                color: const Color(0xFFF97316),
              ),
            ),
            const SizedBox(width: 10),
            Expanded(
              child: _buildMetricCard(
                label: 'TOTAL USAGE',
                value: '${telemetry.totalVolL.toStringAsFixed(2)} L',
                sublabel: 'NVS saved',
                color: const Color(0xFF10B981),
              ),
            ),
          ],
        ),

        const SizedBox(height: 12),

        // Additional Kitchen Devices Card
        Container(
          decoration: BoxDecoration(
            color: const Color(0xFF141C2B),
            borderRadius: BorderRadius.circular(14),
            border: Border.all(color: const Color(0xFF232F42)),
          ),
          padding: const EdgeInsets.all(14),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text(
                'KITCHEN APPLIANCES',
                style: TextStyle(
                  fontSize: 11,
                  fontWeight: FontWeight.w700,
                  color: Color(0xFF64748B),
                  letterSpacing: 0.1,
                ),
              ),
              const SizedBox(height: 10),
              _buildDeviceTile(
                icon: Icons.lightbulb_rounded,
                title: 'Countertop Lights',
                subtitle: 'LED Strip • 80% Brightness',
                isActive: true,
              ),
              const Divider(color: Color(0xFF1F2B3E), height: 16),
              _buildDeviceTile(
                icon: Icons.propane_tank_rounded,
                title: 'Gas & Smoke Detector',
                subtitle: 'Status: Normal (0 ppm)',
                isActive: true,
                activeColor: const Color(0xFF10B981),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildMetricCard({
    required String label,
    required String value,
    required String sublabel,
    required Color color,
  }) {
    return Container(
      decoration: BoxDecoration(
        color: const Color(0xFF141C2B),
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: const Color(0xFF232F42)),
      ),
      padding: const EdgeInsets.all(12),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            label,
            style: TextStyle(fontSize: 9, fontWeight: FontWeight.bold, color: color),
          ),
          const SizedBox(height: 4),
          Text(
            value,
            style: const TextStyle(fontSize: 16, fontWeight: FontWeight.bold, color: Colors.white),
          ),
          const SizedBox(height: 2),
          Text(
            sublabel,
            style: const TextStyle(fontSize: 10, color: Color(0xFF64748B)),
          ),
        ],
      ),
    );
  }

  Widget _buildDeviceTile({
    required IconData icon,
    required String title,
    required String subtitle,
    required bool isActive,
    Color activeColor = const Color(0xFFF97316),
  }) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceBetween,
      children: [
        Row(
          children: [
            Container(
              padding: const EdgeInsets.all(8),
              decoration: BoxDecoration(
                color: const Color(0xFF0E1420),
                borderRadius: BorderRadius.circular(8),
              ),
              child: Icon(icon, size: 20, color: isActive ? activeColor : const Color(0xFF64748B)),
            ),
            const SizedBox(width: 12),
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(title, style: const TextStyle(fontWeight: FontWeight.w600, color: Colors.white)),
                Text(subtitle, style: const TextStyle(fontSize: 11, color: Color(0xFF94A3B8))),
              ],
            ),
          ],
        ),
        Switch(
          value: isActive,
          activeColor: activeColor,
          onChanged: (val) {},
        ),
      ],
    );
  }
}
