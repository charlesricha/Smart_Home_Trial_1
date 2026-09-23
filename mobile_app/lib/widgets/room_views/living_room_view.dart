/**
 * @file living_room_view.dart
 * @brief Dynamic Room View: Living Room (Controls blinds, TV, AC, ambient lights)
 */

import 'package:flutter/material.dart';

class LivingRoomView extends StatefulWidget {
  const LivingRoomView({Key? key}) : super(key: key);

  @override
  State<LivingRoomView> createState() => _LivingRoomViewState();
}

class _LivingRoomViewState extends State<LivingRoomView> {
  bool _blindsOpen = true;
  bool _tvOn = false;
  bool _ambientLight = true;
  double _acTemp = 23.0;

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        // Quick Action Grid (Blinds & TV)
        Row(
          children: [
            Expanded(
              child: _buildTile(
                icon: Icons.blinds_rounded,
                title: 'Smart Blinds',
                subtitle: _blindsOpen ? 'Open (80%)' : 'Closed',
                isActive: _blindsOpen,
                onTap: () => setState(() => _blindsOpen = !_blindsOpen),
              ),
            ),
            const SizedBox(width: 10),
            Expanded(
              child: _buildTile(
                icon: Icons.tv_rounded,
                title: 'Living TV',
                subtitle: _tvOn ? 'Power On' : 'Standby',
                isActive: _tvOn,
                onTap: () => setState(() => _tvOn = !_tvOn),
              ),
            ),
          ],
        ),

        const SizedBox(height: 12),

        // AC & Climate Control Card
        Container(
          decoration: BoxDecoration(
            color: const Color(0xFF141C2B),
            borderRadius: BorderRadius.circular(14),
            border: Border.all(color: const Color(0xFF232F42)),
          ),
          padding: const EdgeInsets.all(16),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Row(
                    children: const [
                      Icon(Icons.ac_unit_rounded, color: Color(0xFF38BDF8), size: 20),
                      SizedBox(width: 8),
                      Text(
                        'AIR CONDITIONING',
                        style: TextStyle(
                          fontSize: 11,
                          fontWeight: FontWeight.bold,
                          color: Color(0xFF64748B),
                          letterSpacing: 0.1,
                        ),
                      ),
                    ],
                  ),
                  Text(
                    '${_acTemp.toStringAsFixed(0)} °C',
                    style: const TextStyle(
                      fontSize: 18,
                      fontWeight: FontWeight.bold,
                      color: Colors.white,
                    ),
                  ),
                ],
              ),
              const SizedBox(height: 8),
              SliderTheme(
                data: SliderTheme.of(context).copyWith(
                  activeTrackColor: const Color(0xFFF97316),
                  inactiveTrackColor: const Color(0xFF334155),
                  thumbColor: const Color(0xFFF97316),
                  overlayColor: const Color(0xFFF97316).withOpacity(0.2),
                ),
                child: Slider(
                  value: _acTemp,
                  min: 16.0,
                  max: 30.0,
                  divisions: 14,
                  onChanged: (val) => setState(() => _acTemp = val),
                ),
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: const [
                  Text('16 °C (Cool)', style: TextStyle(fontSize: 11, color: Color(0xFF64748B))),
                  Text('30 °C (Warm)', style: TextStyle(fontSize: 11, color: Color(0xFF64748B))),
                ],
              ),
            ],
          ),
        ),

        const SizedBox(height: 12),

        // Lighting Tile
        Container(
          decoration: BoxDecoration(
            color: const Color(0xFF141C2B),
            borderRadius: BorderRadius.circular(14),
            border: Border.all(color: const Color(0xFF232F42)),
          ),
          padding: const EdgeInsets.all(14),
          child: Row(
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
                    child: Icon(
                      Icons.light_mode_rounded,
                      color: _ambientLight ? const Color(0xFFF97316) : const Color(0xFF64748B),
                    ),
                  ),
                  const SizedBox(width: 12),
                  Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      const Text('Ambient Strip Lights', style: TextStyle(fontWeight: FontWeight.w600, color: Colors.white)),
                      Text(_ambientLight ? 'Warm Sunset • 70%' : 'Turned off', style: const TextStyle(fontSize: 11, color: Color(0xFF94A3B8))),
                    ],
                  ),
                ],
              ),
              Switch(
                value: _ambientLight,
                activeColor: const Color(0xFFF97316),
                onChanged: (val) => setState(() => _ambientLight = val),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildTile({
    required IconData icon,
    required String title,
    required String subtitle,
    required bool isActive,
    required VoidCallback onTap,
  }) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        decoration: BoxDecoration(
          color: isActive ? const Color(0xFF1E293B) : const Color(0xFF141C2B),
          borderRadius: BorderRadius.circular(14),
          border: Border.all(color: isActive ? const Color(0xFFF97316) : const Color(0xFF232F42)),
        ),
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Icon(icon, color: isActive ? const Color(0xFFF97316) : const Color(0xFF64748B), size: 24),
            const SizedBox(height: 10),
            Text(title, style: const TextStyle(fontWeight: FontWeight.bold, color: Colors.white)),
            const SizedBox(height: 2),
            Text(subtitle, style: const TextStyle(fontSize: 11, color: Color(0xFF94A3B8))),
          ],
        ),
      ),
    );
  }
}
