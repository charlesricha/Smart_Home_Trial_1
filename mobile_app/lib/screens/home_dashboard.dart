/**
 * @file home_dashboard.dart
 * @brief Master Dashboard for NORA Smart Home Companion App
 */

import 'dart:async';
import 'package:flutter/material.dart';
import 'package:intl/intl.dart';
import '../models/telemetry_model.dart';
import '../services/esp32_service.dart';
import '../widgets/music_player_card.dart';
import '../widgets/room_views/kitchen_view.dart';
import '../widgets/room_views/living_room_view.dart';
import '../widgets/room_views/bedroom_view.dart';
import '../widgets/room_views/dining_view.dart';
import '../widgets/room_views/bathroom_view.dart';

class HomeDashboard extends StatefulWidget {
  final Esp32Service service;

  const HomeDashboard({Key? key, required this.service}) : super(key: key);

  @override
  State<HomeDashboard> createState() => _HomeDashboardState();
}

class _HomeDashboardState extends State<HomeDashboard> {
  final List<String> _rooms = [
    'Living Room',
    'Kitchen',
    'Dining',
    'Bedroom',
    'Bathroom',
  ];
  int _selectedRoomIdx = 1; // Default to Kitchen (where our hardware node is!)
  Timer? _clockTimer;
  String _currentTime = '';
  String _currentDate = '';

  @override
  void initState() {
    super.initState();
    _updateClock();
    _clockTimer = Timer.periodic(const Duration(seconds: 1), (timer) {
      _updateClock();
    });
  }

  void _updateClock() {
    final now = DateTime.now();
    setState(() {
      _currentTime = DateFormat('h:mm a').format(now);
      _currentDate = DateFormat('EEE, d MMM').format(now);
    });
  }

  void _showSettingsDialog() {
    final controller = TextEditingController(text: widget.service.baseUrl);
    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        backgroundColor: const Color(0xFF141C2B),
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
        title: const Text('ESP32 Node IP / Hostname', style: TextStyle(color: Colors.white, fontSize: 16)),
        content: TextField(
          controller: controller,
          style: const TextStyle(color: Colors.white),
          decoration: InputDecoration(
            hintText: 'e.g. http://192.168.1.50 or smarthome-kitchen.local',
            hintStyle: const TextStyle(color: Color(0xFF64748B), fontSize: 13),
            enabledBorder: OutlineInputBorder(
              borderSide: const BorderSide(color: Color(0xFF232F42)),
              borderRadius: BorderRadius.circular(10),
            ),
            focusedBorder: OutlineInputBorder(
              borderSide: const BorderSide(color: Color(0xFFF97316)),
              borderRadius: BorderRadius.circular(10),
            ),
          ),
        ),
        actions: [
          TextButton(
            child: const Text('Cancel', style: TextStyle(color: Color(0xFF94A3B8))),
            onPressed: () => Navigator.pop(ctx),
          ),
          ElevatedButton(
            style: ElevatedButton.styleFrom(
              backgroundColor: const Color(0xFFF97316),
              foregroundColor: Colors.white,
            ),
            child: const Text('Save & Connect'),
            onPressed: () {
              widget.service.setBaseUrl(controller.text.trim());
              Navigator.pop(ctx);
            },
          ),
        ],
      ),
    );
  }

  void _showVoiceModal() {
    showModalBottomSheet(
      context: context,
      backgroundColor: Colors.transparent,
      builder: (ctx) => Container(
        padding: const EdgeInsets.all(24),
        decoration: const BoxDecoration(
          color: Color(0xFF141C2B),
          borderRadius: BorderRadius.vertical(top: Radius.circular(24)),
          border: Border(top: BorderSide(color: Color(0xFF232F42))),
        ),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Container(
              width: 40,
              height: 4,
              decoration: BoxDecoration(
                color: const Color(0xFF334155),
                borderRadius: BorderRadius.circular(2),
              ),
            ),
            const SizedBox(height: 20),
            Container(
              width: 70,
              height: 70,
              decoration: BoxDecoration(
                shape: BoxShape.circle,
                color: const Color(0xFFF97316),
                boxShadow: [
                  BoxShadow(
                    color: const Color(0xFFF97316).withOpacity(0.4),
                    blurRadius: 20,
                    spreadRadius: 4,
                  ),
                ],
              ),
              child: const Icon(Icons.mic_rounded, color: Colors.white, size: 36),
            ),
            const SizedBox(height: 16),
            const Text(
              'Listening...',
              style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white),
            ),
            const SizedBox(height: 6),
            const Text(
              '"Hey Nora, open the kitchen tap"',
              style: TextStyle(fontSize: 14, color: Color(0xFF94A3B8), fontStyle: FontStyle.italic),
            ),
            const SizedBox(height: 20),
          ],
        ),
      ),
    );
  }

  @override
  void dispose() {
    _clockTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return AnimatedBuilder(
      animation: widget.service,
      builder: (context, _) {
        final telemetry = widget.service.telemetry;
        final isConnected = widget.service.isConnected;
        final selectedRoom = _rooms[_selectedRoomIdx];

        return Scaffold(
          backgroundColor: const Color(0xFF0B0F19),
          appBar: AppBar(
            backgroundColor: const Color(0xFF0B0F19),
            elevation: 0,
            title: Row(
              children: [
                Container(
                  padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 3),
                  decoration: BoxDecoration(
                    color: const Color(0xFFF97316),
                    borderRadius: BorderRadius.circular(6),
                  ),
                  child: const Text(
                    'NORA',
                    style: TextStyle(
                      fontSize: 13,
                      fontWeight: FontWeight.w900,
                      color: Colors.white,
                      letterSpacing: 0.5,
                    ),
                  ),
                ),
                const SizedBox(width: 10),
                const Text(
                  'Smart Home',
                  style: TextStyle(fontSize: 17, fontWeight: FontWeight.bold, color: Colors.white),
                ),
              ],
            ),
            actions: [
              // Connection Status Badge
              GestureDetector(
                onTap: _showSettingsDialog,
                child: Container(
                  margin: const EdgeInsets.only(right: 14),
                  padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 5),
                  decoration: BoxDecoration(
                    color: const Color(0xFF141C2B),
                    borderRadius: BorderRadius.circular(20),
                    border: Border.all(
                      color: isConnected ? const Color(0xFF10B981) : const Color(0xFFEF4444),
                    ),
                  ),
                  child: Row(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      Container(
                        width: 8,
                        height: 8,
                        decoration: BoxDecoration(
                          shape: BoxShape.circle,
                          color: isConnected ? const Color(0xFF10B981) : const Color(0xFFEF4444),
                        ),
                      ),
                      const SizedBox(width: 6),
                      Text(
                        isConnected ? 'Online' : 'Offline',
                        style: TextStyle(
                          fontSize: 11,
                          fontWeight: FontWeight.w600,
                          color: isConnected ? const Color(0xFF10B981) : const Color(0xFFEF4444),
                        ),
                      ),
                      const SizedBox(width: 4),
                      const Icon(Icons.settings_rounded, size: 13, color: Color(0xFF94A3B8)),
                    ],
                  ),
                ),
              ),
            ],
          ),

          body: ListView(
            padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
            children: [
              // Clock & Climate Summary Bar
              Container(
                decoration: BoxDecoration(
                  color: const Color(0xFF141C2B),
                  borderRadius: BorderRadius.circular(14),
                  border: Border.all(color: const Color(0xFF232F42)),
                ),
                padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 12),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(_currentTime, style: const TextStyle(fontSize: 20, fontWeight: FontWeight.w800, color: Colors.white)),
                        Text(_currentDate, style: const TextStyle(fontSize: 12, color: Color(0xFF94A3B8))),
                      ],
                    ),
                    Row(
                      children: [
                        _buildClimateChip(icon: Icons.thermostat_rounded, label: '24.5 °C', color: const Color(0xFFFB923C)),
                        const SizedBox(width: 12),
                        _buildClimateChip(icon: Icons.water_drop_rounded, label: '58 %', color: const Color(0xFF38BDF8)),
                      ],
                    ),
                  ],
                ),
              ),

              const SizedBox(height: 12),

              // Block 1: Music Player Card
              MusicPlayerCard(activeRoom: selectedRoom),

              const SizedBox(height: 16),

              // Block 3: Dynamic Room Tabs
              SizedBox(
                height: 40,
                child: ListView.separated(
                  scrollDirection: Axis.horizontal,
                  itemCount: _rooms.length,
                  separatorBuilder: (_, __) => const SizedBox(width: 8),
                  itemBuilder: (ctx, idx) {
                    final isSel = idx == _selectedRoomIdx;
                    return GestureDetector(
                      onTap: () => setState(() => _selectedRoomIdx = idx),
                      child: AnimatedContainer(
                        duration: const Duration(milliseconds: 200),
                        padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
                        decoration: BoxDecoration(
                          color: isSel ? const Color(0xFFF97316) : const Color(0xFF141C2B),
                          borderRadius: BorderRadius.circular(10),
                          border: Border.all(
                            color: isSel ? const Color(0xFFFB923C) : const Color(0xFF232F42),
                          ),
                          boxShadow: isSel
                              ? [BoxShadow(color: const Color(0xFFF97316).withOpacity(0.35), blurRadius: 8)]
                              : [],
                        ),
                        child: Center(
                          child: Text(
                            _rooms[idx],
                            style: TextStyle(
                              fontSize: 13,
                              fontWeight: FontWeight.bold,
                              color: isSel ? Colors.white : const Color(0xFF94A3B8),
                            ),
                          ),
                        ),
                      ),
                    );
                  },
                ),
              ),

              const SizedBox(height: 14),

              // Dynamic Room Content Area
              _buildSelectedRoomView(selectedRoom, telemetry, widget.service),

              const SizedBox(height: 80), // Padding for Floating Action Button
            ],
          ),

          // NORA Floating Voice Trigger Button
          floatingActionButton: FloatingActionButton.extended(
            backgroundColor: const Color(0xFFF97316),
            elevation: 8,
            onPressed: _showVoiceModal,
            icon: const Icon(Icons.mic_rounded, color: Colors.white),
            label: const Text('Hey Nora', style: TextStyle(fontWeight: FontWeight.bold, color: Colors.white)),
          ),
        );
      },
    );
  }

  Widget _buildClimateChip({
    required IconData icon,
    required String label,
    required Color color,
  }) {
    return Row(
      children: [
        Icon(icon, size: 16, color: color),
        const SizedBox(width: 4),
        Text(label, style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 13, color: Colors.white)),
      ],
    );
  }

  Widget _buildSelectedRoomView(String roomName, NodeTelemetry telemetry, Esp32Service service) {
    switch (roomName) {
      case 'Kitchen':
        return KitchenView(telemetry: telemetry, service: service);
      case 'Living Room':
        return const LivingRoomView();
      case 'Bedroom':
        return const BedroomView();
      case 'Dining':
        return const DiningView();
      case 'Bathroom':
        return const BathroomView();
      default:
        return const SizedBox();
    }
  }
}
