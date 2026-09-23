/**
 * @file bathroom_view.dart
 * @brief Dynamic Room View: Bathroom (Controls water heater, exhaust, vanity lights)
 */

import 'package:flutter/material.dart';

class BathroomView extends StatefulWidget {
  const BathroomView({Key? key}) : super(key: key);

  @override
  State<BathroomView> createState() => _BathroomViewState();
}

class _BathroomViewState extends State<BathroomView> {
  bool _heaterOn = true;
  bool _exhaustOn = false;
  bool _vanityLight = true;
  double _waterTemp = 50.0;

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        // Water Heater / Geyser Card
        Container(
          decoration: BoxDecoration(
            color: const Color(0xFF141C2B),
            borderRadius: BorderRadius.circular(14),
            border: Border.all(color: _heaterOn ? const Color(0xFFF97316) : const Color(0xFF232F42)),
          ),
          padding: const EdgeInsets.all(16),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
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
                          Icons.water_drop_rounded,
                          color: _heaterOn ? const Color(0xFFF97316) : const Color(0xFF64748B),
                        ),
                      ),
                      const SizedBox(width: 12),
                      Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          const Text('Water Heater (Geyser)', style: TextStyle(fontWeight: FontWeight.bold, color: Colors.white, fontSize: 15)),
                          Text(_heaterOn ? 'Heating • Target ${_waterTemp.toInt()}°C' : 'Standby / Off', style: const TextStyle(fontSize: 11, color: Color(0xFF94A3B8))),
                        ],
                      ),
                    ],
                  ),
                  Switch(
                    value: _heaterOn,
                    activeColor: const Color(0xFFF97316),
                    onChanged: (val) => setState(() => _heaterOn = val),
                  ),
                ],
              ),
              if (_heaterOn) ...[
                const SizedBox(height: 12),
                SliderTheme(
                  data: SliderTheme.of(context).copyWith(
                    activeTrackColor: const Color(0xFFF97316),
                    inactiveTrackColor: const Color(0xFF334155),
                    thumbColor: const Color(0xFFF97316),
                  ),
                  child: Slider(
                    value: _waterTemp,
                    min: 35.0,
                    max: 65.0,
                    divisions: 6,
                    onChanged: (val) => setState(() => _waterTemp = val),
                  ),
                ),
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: const [
                    Text('35 °C (Warm)', style: TextStyle(fontSize: 11, color: Color(0xFF64748B))),
                    Text('65 °C (Hot)', style: TextStyle(fontSize: 11, color: Color(0xFF64748B))),
                  ],
                ),
              ],
            ],
          ),
        ),

        const SizedBox(height: 12),

        // Exhaust Fan & Vanity Light
        Row(
          children: [
            Expanded(
              child: _buildTile(
                icon: Icons.air_rounded,
                title: 'Exhaust Fan',
                subtitle: _exhaustOn ? 'High Speed' : 'Off',
                isActive: _exhaustOn,
                onTap: () => setState(() => _exhaustOn = !_exhaustOn),
              ),
            ),
            const SizedBox(width: 10),
            Expanded(
              child: _buildTile(
                icon: Icons.wb_incandescent_rounded,
                title: 'Vanity Mirror',
                subtitle: _vanityLight ? 'Bright White' : 'Off',
                isActive: _vanityLight,
                onTap: () => setState(() => _vanityLight = !_vanityLight),
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
