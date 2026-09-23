/**
 * @file main.dart
 * @brief NORA Smart Home Companion App Entry Point
 */

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'services/esp32_service.dart';
import 'screens/home_dashboard.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();

  // Dark immersive status bar
  SystemChrome.setSystemUIOverlayStyle(
    const SystemUiOverlayStyle(
      statusBarColor: Colors.transparent,
      statusBarIconBrightness: Brightness.light,
      systemNavigationBarColor: Color(0xFF0B0F19),
      systemNavigationBarIconBrightness: Brightness.light,
    ),
  );

  final esp32Service = Esp32Service();
  runApp(NoraSmartHomeApp(service: esp32Service));
}

class NoraSmartHomeApp extends StatelessWidget {
  final Esp32Service service;

  const NoraSmartHomeApp({Key? key, required this.service}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'NORA Smart Home',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        brightness: Brightness.dark,
        scaffoldBackgroundColor: const Color(0xFF0B0F19),
        cardColor: const Color(0xFF141C2B),
        primaryColor: const Color(0xFFF97316),
        colorScheme: const ColorScheme.dark(
          primary: Color(0xFFF97316),
          secondary: Color(0xFF38BDF8),
          surface: Color(0xFF141C2B),
          background: Color(0xFF0B0F19),
        ),
        fontFamily: 'Roboto',
      ),
      home: HomeDashboard(service: service),
    );
  }
}
