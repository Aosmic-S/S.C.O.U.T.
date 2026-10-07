/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: README.md · Purpose: Main project documentation overview
 */

<!--
  Copyright 2026 Absolute Tech
  Licensed under the Apache License, Version 2.0
-->

<div align="center">

# 🛰️ S.C.O.U.T.

### **S**afety **C**ontrol & **O**bservation **U**nit **T**ech

**A competition-ready rescue bot firmware OS for post-disaster environments.**

**Built by Team Absolute Tech**

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-ESP32%20LOLIN32-red.svg)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino%20%2B%20FreeRTOS-teal.svg)](https://www.freertos.org/)
[![Build](https://img.shields.io/badge/Build-PlatformIO-orange.svg)](https://platformio.org/)
[![CI](https://github.com/AbsoluteTech/SCOUT/actions/workflows/build.yml/badge.svg)](.github/workflows/build.yml)
[![Version](https://img.shields.io/badge/Version-v0.27.0--OP-blue.svg)](CHANGELOG.md)
[![Status](https://img.shields.io/badge/Status-Active%20Development-yellow.svg)]()
[![Team](https://img.shields.io/badge/Team-Absolute%20Tech-purple.svg)]()

*Because in a disaster zone, every second and every sensor reading matters.*

[Features](#-features) • [Architecture](#-system-architecture) • [Hardware](#-hardware-requirements) • [API Guide](docs/API.md) • [Wiring](docs/WIRING.md) • [License](#-license)

</div>

---

## 📖 Overview

**S.C.O.U.T. (v0.27.0 "OP")** is a modular firmware framework — a rescue bot "OS" designed for an ESP32 LOLIN32 controller operating in hazardous post-disaster environments.

It continuously monitors toxic gases (MQ2, MQ135), internal and external environmental conditions (dual DHT11), tracks battery status via a precision resistor divider on ADC1, supports Bluetooth gamepad control (CLAW Shoot V3), and provides a non-blocking web dashboard served from LittleFS over an open FPV camera access point.

---

## ✨ Key Features

- **Dual Gas Detection**: Direct ADC1 sampling of MQ2 (smoke/LPG) and MQ135 (air quality) with logarithmic PPM calculation and calibration routines.
- **Dual Environmental Sensing**: Dual DHT11 monitoring internal chassis and external ambient conditions.
- **CLAW Shoot V3 Bluetooth Host**: Direct BT HID gamepad driving via Bluepad32 in Standard Mode.
- **Fail-Safe Motor Driver**: L298N motor control with dead-man timeout (3s), stall protection (PWM > 50% for 2s + batt < 10V), and emergency stop.
- **Non-Blocking Web Dashboard**: ESPAsyncWebServer single-page application served from LittleFS with dark UI, 14 telemetry cards, and gzip support.
- **Captive Portal DNS**: All web requests automatically redirected to the S.C.O.U.T. control dashboard.
- **Mission Black-Box Logging**: Telemetry recorded to `/logs/mission.csv` in LittleFS with brownout tracking in RTC memory.

---

## 🔧 Hardware Requirements & Pin Mapping

| Component | ESP32 LOLIN32 Pin | Notes |
|-----------|-------------------|-------|
| Motor ENA / IN1 / IN2 | GPIO 13, 12, 14 | Left Motor Pair PWM & Dir |
| Motor ENB / IN3 / IN4 | GPIO 27, 26, 25 | Right Motor Pair PWM & Dir |
| MQ2 Gas Sensor | GPIO 34 | Analog ADC1 ONLY |
| MQ135 Air Quality | GPIO 35 | Analog ADC1 ONLY |
| Battery Divider | GPIO 32 | ADC1, R1=100kΩ, R2=22kΩ |
| Internal DHT11 | GPIO 4 | Digital (10kΩ pull-up) |
| External DHT11 | GPIO 2 | Digital (10kΩ pull-up) |
| Active Buzzer | GPIO 15 | Audio alarms & beeps |
| Status LED | GPIO 5 | Onboard LED |

---

## ⚙️ Quick Start & Build

1. Clone repository:
   ```bash
   git clone https://github.com/AbsoluteTech/SCOUT.git
   cd SCOUT
   ```
2. Copy configuration:
   ```bash
   cp firmware/config.example.h firmware/config.h
   ```
3. Build with PlatformIO:
   ```bash
   pio run -e lolin32
   ```

---

## 📜 Documentation

- [System Architecture](docs/ARCHITECTURE.md)
- [Wiring & Pinouts](docs/WIRING.md)
- [REST API Specification](docs/API.md)
- [Sensor Calibration](docs/CALIBRATION.md)

---

## ⚖️ License

Licensed under the **Apache License, Version 2.0**. See [LICENSE](LICENSE) and [NOTICE](NOTICE) for details.
Copyright 2026 Absolute Tech.
