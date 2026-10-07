# Changelog

All notable changes to S.C.O.U.T. (Safety Control & Observation Unit Tech) will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.27.0] - 2026-03-30 — "OP"

### Added
- Upgraded MCU support to ESP32 LOLIN32 (CP2102) with FreeRTOS multitasking.
- Direct dual MQ gas sensing (MQ2 on GPIO 34, MQ135 on GPIO 35) via ADC1 with PPM conversion.
- Dual DHT11 temperature & humidity monitoring (Internal GPIO 4, External GPIO 2).
- Battery monitor with 100kΩ/22kΩ divider on GPIO 32 (11dB ADC attenuation).
- L298N motor driver integration with PWM speed control, dead-man switch (3s timeout), and stall detection.
- CLAW Shoot V3 Bluetooth gamepad host support via Bluepad32 in Standard Mode.
- ESPAsyncWebServer non-blocking web engine with gzip pre-compression and captive portal.
- Single-page vanilla web dashboard with dark theme (#0a0e1a), monospace metrics, and 14 distinct cards/sections.
- Autonomous operational modes: Manual, Gas Sniffer, Perimeter Scan, Return-To-Launch (RTL).
- Mission black-box logging to LittleFS with CSV export (`/api/logs.csv`) and brownout RTC memory tracking.
- GitHub Actions CI workflow targeting `pio run -e lolin32`.
