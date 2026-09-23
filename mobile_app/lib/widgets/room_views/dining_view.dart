/**
 * @file dining_view.dart
 * @brief Dynamic Room View: Dining (Controls chandelier, mood lights, dining volume)
 */

import 'package:flutter/material.dart';

class DiningView extends StatefulWidget {
  const DiningView({Key? key}) : super(key: key);

  @override
  State<DiningView> createState() => _DiningViewState();
}

class _DiningViewState extends State<DiningView> {
  bool _chandelier = true;
  bool _moodLights = false;
  double _volume = 0.6;

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Row(
          children: [
            Expanded(
              child: _buildTile(
                icon: Icons.light_rounded,
                title: 'Chandelier',
                subtitle: _chandelier ? 'Warm (100%)' : 'Off',
                isActive: _chandelier,
                onTap: () => setState(() => _chandelier = !_chandelier),
              ),
            ),
            const SizedBox(width: 10),
            Expanded(
              child: _buildTile(
                icon: Icons.palette_rounded,
                title: 'Mood Scene',
                subtitle: _moodLights ? 'Dinner Preset' : 'Standard',
                isActive: _moodLights,
                onTap: () => setState(() => _moodLights = !_moodLights),
              ),
            ),
          ],
        ),
        const SizedBox(height: 12),
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
                  const Text('DINING SPEAKER VOLUME', style: TextStyle(fontSize: 11, fontWeight: FontWeight.bold, color: Color(0xFF64748B))),
                  Text('${(_volume * 100).toInt()}%', style: const TextStyle(fontWeight: FontWeight.bold, color: Colors.white)),
                ],
              ),
              const SizedBox(height: 6),
              SliderTheme(
                data: SliderTheme.of(context).copyWith(
                  activeTrackColor: const Color(0xFFF97316),
                  inactiveTrackColor: const Color(0xFF334155),
                  thumbColor: const Color(0xFFF97316),
                ),
                child: Slider(
                  value: _volume,
                  onChanged: (val) => setState(() => _volume = val),
                ),
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
