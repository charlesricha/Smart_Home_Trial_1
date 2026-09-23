/**
 * @file music_player_card.dart
 * @brief Block 1: Music Player Card with active room speaker routing
 */

import 'dart:async';
import 'package:flutter/material.dart';

class MusicTrack {
  final String title;
  final String artist;
  final int durationSec;

  const MusicTrack({
    required this.title,
    required this.artist,
    required this.durationSec,
  });
}

class MusicPlayerCard extends StatefulWidget {
  final String activeRoom;

  const MusicPlayerCard({
    Key? key,
    required this.activeRoom,
  }) : super(key: key);

  @override
  State<MusicPlayerCard> createState() => _MusicPlayerCardState();
}

class _MusicPlayerCardState extends State<MusicPlayerCard> {
  static const List<MusicTrack> _playlist = [
    MusicTrack(title: 'Blinding Lights', artist: 'The Weeknd', durationSec: 200),
    MusicTrack(title: 'As It Was', artist: 'Harry Styles', durationSec: 167),
    MusicTrack(title: 'Levitating', artist: 'Dua Lipa', durationSec: 203),
    MusicTrack(title: 'Shape of You', artist: 'Ed Sheeran', durationSec: 233),
  ];

  int _currentTrackIdx = 0;
  bool _isPlaying = true;
  int _elapsedSec = 45;
  Timer? _progressTimer;

  @override
  void initState() {
    super.initState();
    _startTicker();
  }

  void _startTicker() {
    _progressTimer = Timer.periodic(const Duration(seconds: 1), (timer) {
      if (_isPlaying) {
        setState(() {
          _elapsedSec++;
          if (_elapsedSec >= _playlist[_currentTrackIdx].durationSec) {
            _nextTrack();
          }
        });
      }
    });
  }

  void _togglePlay() {
    setState(() {
      _isPlaying = !_isPlaying;
    });
  }

  void _nextTrack() {
    setState(() {
      _currentTrackIdx = (_currentTrackIdx + 1) % _playlist.length;
      _elapsedSec = 0;
    });
  }

  void _prevTrack() {
    setState(() {
      _currentTrackIdx = (_currentTrackIdx == 0)
          ? _playlist.length - 1
          : _currentTrackIdx - 1;
      _elapsedSec = 0;
    });
  }

  String _formatTime(int sec) {
    final m = sec ~/ 60;
    final s = sec % 60;
    return '$m:${s.toString().padLeft(2, '0')}';
  }

  @override
  void dispose() {
    _progressTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final track = _playlist[_currentTrackIdx];
    final progress = (_elapsedSec / track.durationSec).clamp(0.0, 1.0);

    return Container(
      decoration: BoxDecoration(
        color: const Color(0xFF141C2B),
        borderRadius: BorderRadius.circular(14),
        border: Border.all(color: const Color(0xFF232F42)),
      ),
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Header Row: Active Speaker Badge & Playing status
          Row(
            mainAxisAlignment: MainAxisAlignment.between,
            children: [
              Container(
                padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
                decoration: BoxDecoration(
                  color: const Color(0xFF1E293B),
                  borderRadius: BorderRadius.circular(20),
                  border: Border.all(color: const Color(0xFFF97316)), // Orange accent
                ),
                child: Row(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    const Icon(Icons.volume_up_rounded, size: 14, color: Color(0xFFF97316)),
                    const SizedBox(width: 6),
                    Text(
                      widget.activeRoom,
                      style: const TextStyle(
                        fontSize: 12,
                        fontWeight: FontWeight.w600,
                        color: Color(0xFFF97316),
                      ),
                    ),
                  ],
                ),
              ),
              Row(
                children: [
                  Icon(
                    _isPlaying ? Icons.play_arrow_rounded : Icons.pause_rounded,
                    size: 14,
                    color: _isPlaying ? const Color(0xFF10B981) : const Color(0xFF94A3B8),
                  ),
                  const SizedBox(width: 4),
                  Text(
                    _isPlaying ? 'Playing' : 'Paused',
                    style: TextStyle(
                      fontSize: 12,
                      fontWeight: FontWeight.w600,
                      color: _isPlaying ? const Color(0xFF10B981) : const Color(0xFF94A3B8),
                    ),
                  ),
                ],
              ),
            ],
          ),

          const SizedBox(height: 12),

          // Track Title & Artist
          Text(
            track.title,
            style: const TextStyle(
              fontSize: 18,
              fontWeight: FontWeight.bold,
              color: Colors.white,
            ),
          ),
          const SizedBox(height: 2),
          Text(
            track.artist,
            style: const TextStyle(
              fontSize: 13,
              color: Color(0xFF94A3B8),
            ),
          ),

          const SizedBox(height: 12),

          // Controls & Progress Row
          Row(
            children: [
              IconButton(
                icon: const Icon(Icons.skip_previous_rounded, color: Colors.white),
                iconSize: 22,
                padding: EdgeInsets.zero,
                constraints: const BoxConstraints(),
                onPressed: _prevTrack,
              ),
              const SizedBox(width: 12),
              GestureDetector(
                onTap: _togglePlay,
                child: Container(
                  width: 36,
                  height: 36,
                  decoration: BoxDecoration(
                    color: const Color(0xFFF97316),
                    borderRadius: BorderRadius.circular(10),
                    boxShadow: [
                      BoxShadow(
                        color: const Color(0xFFF97316).withOpacity(0.4),
                        blurRadius: 10,
                      ),
                    ],
                  ),
                  child: Icon(
                    _isPlaying ? Icons.pause_rounded : Icons.play_arrow_rounded,
                    color: Colors.white,
                    size: 20,
                  ),
                ),
              ),
              const SizedBox(width: 12),
              IconButton(
                icon: const Icon(Icons.skip_next_rounded, color: Colors.white),
                iconSize: 22,
                padding: EdgeInsets.zero,
                constraints: const BoxConstraints(),
                onPressed: _nextTrack,
              ),
              const SizedBox(width: 14),

              // Progress Bar
              Expanded(
                child: ClipRRect(
                  borderRadius: BorderRadius.circular(3),
                  child: LinearProgressIndicator(
                    value: progress,
                    minHeight: 5,
                    backgroundColor: const Color(0xFF334155),
                    valueColor: const AlwaysStoppedAnimation<Color>(Color(0xFFF97316)),
                  ),
                ),
              ),
              const SizedBox(width: 10),
              Text(
                _formatTime(_elapsedSec),
                style: const TextStyle(
                  fontSize: 11,
                  color: Color(0xFF64748B),
                  fontFeatures: [FontFeature.tabularFigures()],
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}
