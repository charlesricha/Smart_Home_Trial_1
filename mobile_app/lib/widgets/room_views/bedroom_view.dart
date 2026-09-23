/**
 * @file bedroom_view.dart
 * @brief Dynamic Room View: Bedroom (Controls curtains, night lamps, fan, sleep mode)
 */

import 'package:flutter/material.dart';

class BedroomView extends StatefulWidget {
  const BedroomView({Key? key}) : super(key: key);

  @override
  State<BedroomView> createState() => _BedroomViewState();
}

class _BedroomViewState extends State<BedroomView> {
  bool _nightLamp = true;
  bool _curtainsClosed = false;
  bool _sleepMode = false;
  int _fanSpeed = 2; // 0=off, 1=low, 2=med, 3=high

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        // Sleep Mode Master Card
        Container(
          decoration: BoxDecoration(
            color: _sleepMode ? const Color(0xFF1E293B) : const Color(0xFF141C2B),
            borderRadius: BorderRadius.circular(14),
            border: Border.all(color: _sleepMode ? const Color(0xFFF97316) : const Color(0xFF232F42)),
          ),
          padding: const EdgeInsets.all(16),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Row(
                children: [
                  Container(
                    padding: const EdgeInsets.all(10),
                    decoration: BoxDecoration(
                      color: const Color(0xFF0E1420),
                      borderRadius: BorderRadius.circular(10),
                    ),
                    child: Icon(
                      Icons.bedtime_rounded,
                      color: _sleepMode ? const Color(0xFFF97316) : const Color(0xFF64748B),
                    ),
                  ),
                  const SizedBox(width: 12),
                  Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      const Text('Sleep Mode', style: TextStyle(fontWeight: FontWeight.bold, fontSize: 16, color: Colors.white)),
                      Text(
                        _sleepMode ? 'Dim lights, close curtains, set AC 22°' : 'Tap to activate bedtime scene',
                        style: const TextStyle(fontSize: 11, color: Color(0xFF94A3B8)),
                      ),
                    ],
                  ),
                ],
              ),
              Switch(
                value: _sleepMode,
                activeColor: const Color(0xFFF97316),
                onChanged: (val) {
                  setState(() {
                    _sleepMode = val;
                    if (val) {
                      _curtainsClosed = true;
                      _nightLamp = true;
                    }
                  });
                },
              ),
            ],
          ),
        ),

        const SizedBox(height: 12),

        // Ceiling Fan Speed Card
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
                      Icon(Icons.mode_fan_off_rounded, color: Color(0xFF38BDF8), size: 20),
                      SizedBox(width: 8),
                      Text('CEILING FAN', style: TextStyle(fontSize: 11, fontWeight: FontWeight.bold, color: Color(0xFF64748B))),
                    ],
                  ),
                  Text(_fanSpeed == 0 ? 'OFF' : 'Speed $_fanSpeed', style: const TextStyle(fontWeight: FontWeight.bold, color: Colors.white)),
                ],
              ),
              const SizedBox(height: 12),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [0, 1, 2, 3].map((speed) {
                  final isSel = _fanSpeed == speed;
                  return Expanded(
                    child: Padding(
                      padding: const EdgeInsets.symmetric(horizontal: 4),
                      child: GestureDetector(
                        onTap: () => setState(() => _fanSpeed = speed),
                        child: Container(
                          padding: const EdgeInsets.symmetric(vertical: 8),
                          decoration: BoxDecoration(
                            color: isSel ? const Color(0xFFF97316) : const Color(0xFF1E293B),
                            borderRadius: BorderRadius.circular(8),
                            border: Border.all(color: isSel ? const Color(0xFFF97316) : const Color(0xFF334155)),
                          ),
                          alignment: Alignment.center,
                          child: Text(
                            speed == 0 ? 'OFF' : '$speed',
                            style: TextStyle(
                              fontWeight: FontWeight.bold,
                              color: isSel ? Colors.white : const Color(0xFF94A3B8),
                            ),
                          ),
                        ),
                      ),
                    ),
                  );
                }).toList(),
              ),
            ],
          ),
        ),

        const SizedBox(height: 12),

        // Curtains & Night Lamp Row
        Row(
          children: [
            Expanded(
              child: _buildTile(
                icon: Icons.curtains_rounded,
                title: 'Curtains',
                subtitle: _curtainsClosed ? 'Closed' : 'Open',
                isActive: _curtainsClosed,
                onTap: () => setState(() => _curtainsClosed = !_curtainsClosed),
              ),
            ),
            const SizedBox(width: 10),
            Expanded(
              child: _buildTile(
                icon: Icons.nightlight_round,
                title: 'Night Lamp',
                subtitle: _nightLamp ? 'Soft Amber (20%)' : 'Off',
                isActive: _nightLamp,
                onTap: () => setState(() => _nightLamp = !_nightLamp),
              ),
            ),
          ],
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
