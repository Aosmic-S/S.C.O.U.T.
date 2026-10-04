<!--
  Copyright 2026 Absolute Tech

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
-->

<div align="center">

#  S.C.O.U.T.

### **S**afety **C**ontrol & **O**bservation **U**nit **T**ech

**A custom ESP8266-based rescue bot OS for post-disaster environments.**

**Built by Team Absolute Tech**

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Platform](https://img.shields.io/badge/Platform-ESP8266-red.svg)](https://www.espressif.com/en/products/socs/esp8266)
[![Framework](https://img.shields.io/badge/Framework-Arduino%20%2B%20FreeRTOS-teal.svg)](https://www.freertos.org/)
[![Version](https://img.shields.io/badge/Version-v0.1.0--Pup-orange.svg)](CHANGELOG.md)
[![Status](https://img.shields.io/badge/Status-Active%20Development-yellow.svg)]()
[![Team](https://img.shields.io/badge/Team-Absolute%20Tech-purple.svg)]()

*Because in a disaster zone, every second and every sensor reading matters.*

[Features](#-features) • [Architecture](#-system-architecture) • [Hardware](#-hardware-requirements) • [Setup](#-getting-started) • [Roadmap](#-roadmap) • [License](#-license)

</div>

---

## 📖 Overview

**S.C.O.U.T.** is a lightweight, modular firmware framework — essentially a custom "operating system" — designed for a NodeMCU ESP8266-based rescue robot operating in post-disaster environments.

It continuously monitors air quality, tracks environmental conditions, and provides WiFi-based remote control through an innovative networking approach: **the bot connects as a station to an FPV camera's open access point**, allowing any user on that network to view the live video feed and control the bot through an on-board web dashboard.

Built with **FreeRTOS** task scheduling, fail-safe motor control, and mission data logging for professional competition use.

> **Developed by Team Absolute Tech** — driven by a mission to make disaster response faster, safer, and smarter.

---

## ✨ Features

### 🚨 Safety & Rescue Core
- **Dual Gas Detection** — MQ2 (smoke/LPG/methane) + MQ135 (CO2/NH3/benzene)
- **Environmental Monitoring** — DHT11/DHT22 temperature & humidity
- **Threshold Alerts** — LED + buzzer + web notification on danger levels
- **Auto-Retreat Mode** — Bot returns to start point if gas exceeds critical level

### 🎮 Control & UX
- **Zero-Setup Networking** — Connects to camera's AP; users just join and browse
- **Captive Portal** — Any URL typed redirects to the S.C.O.U.T. control page
- **Multi-Mode Control** — Manual, assisted, and autonomous operation
- **Emergency Stop** — Software + optional hardware kill switch

### 🧠 Autonomy
- **Gas Sniffer Mode** — Auto-advances and samples air
- **Perimeter Scan** — Rotates and samples at intervals
- **Return-to-Home** — Retraces path on command or low battery

### 🔋 Reliability
- **FreeRTOS Task Scheduling** — Non-blocking, priority-based execution
- **Watchdog Timer** — Auto-reset on firmware hang
- **Dead-Man Switch** — Motors stop if no command received in 3s
- **Battery Monitoring** — Live voltage + low-battery auto-return
- **Mission Data Logging** — LittleFS black-box with CSV export

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     USER'S PHONE                        │
│  (Camera Stream + S.C.O.U.T. Control Dashboard)         │
└───────────────────────┬─────────────────────────────────┘
                        │
                        │ (connects to open AP)
                        ▼
              ┌─────────────────────┐
              │   E88 CAMERA AP     │
              │   (Open Network)    │
              └──────────┬──────────┘
                         │
                         │ (ESP connects as STA)
                         ▼
        ┌────────────────────────────────┐
        │      S.C.O.U.T. FIRMWARE       │
        │      (NodeMCU ESP8266)         │
        ├────────────────────────────────┤
        │  Layer 3: Application          │
        │    • Web Server (HTTP)         │
        │    • Captive Portal (DNS)      │
        │    • Command Parser            │
        ├────────────────────────────────┤
        │  Layer 2: FreeRTOS Kernel      │
        │    • Motor Task    (High)      │
        │    • Web Server    (Medium)    │
        │    • Sensor Poll   (Medium)    │
        │    • WiFi Monitor  (Low)       │
        ├────────────────────────────────┤
        │  Layer 1: Hardware Abstraction │
        │    • motor_l298n               │
        │    • sensor_mq2 / mq135 / dht  │
        │    • battery_monitor           │
        └────────────┬───────────────────┘
                     │
        ┌────────────┼────────────┬─────────────┐
        ▼            ▼            ▼             ▼
   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌──────────┐
   │ L298N   │  │  MQ2    │  │  MQ135  │  │   DHT    │
   │ + 4     │  │  Gas    │  │   Air   │  │  Temp/   │
   │ Motors  │  │ Sensor  │  │ Quality │  │  Humid   │
   └─────────┘  └─────────┘  └─────────┘  └──────────┘
```

---

## 🔧 Hardware Requirements

### Core Components

| Component | Purpose | Quantity |
|-----------|---------|----------|
| NodeMCU ESP8266 (v12E) | Main controller | 1 |
| L298N Motor Driver | Dual H-bridge for motors | 1 |
| DC Gear Motors | Locomotion | 4 |
| MQ2 Sensor | Smoke / LPG / Methane | 1 |
| MQ135 Sensor | Air quality / CO2 / NH3 | 1 |
| DHT11 or DHT22 | Temperature & Humidity | 1 |
| E88 FPV Camera | Video feed (self-contained AP) | 1 |
| 12V Li-ion / LiPo Battery | Main power | 1 |
| Buck Converter (12V→5V) | Logic power | 1 |
| CD74HC4051 (optional) | Analog multiplexer for multiple analog sensors | 1 |
| 1000µF Capacitor | Brownout protection | 1 |
| Buzzer + LEDs | Alerts & status | — |

### Pin Mapping (Default)

| Function | ESP8266 Pin | Notes |
|----------|-------------|-------|
| Motor IN1–IN4 | D1, D2, D5, D6 | L298N control |
| Motor ENA / ENB | D3, D7 | PWM speed |
| MQ2 Analog | A0 (via mux) | Requires multiplexer |
| MQ135 Analog | A0 (via mux) | Requires multiplexer |
| DHT Data | D4 | Pull-up 10kΩ |
| Battery Monitor | A0 (via divider) | Voltage divider |
| Buzzer | D0 | Active buzzer |
| Status LED | D8 | Built-in LED |

> ⚠️ **Important:** The ESP8266 has only **one ADC pin (A0)**. Use a CD74HC4051 analog multiplexer to read multiple analog sensors.

---

## ⚙️ Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) 2.x **or** [PlatformIO](https://platformio.org/)
- ESP8266 Board Package (`http://arduino.esp8266.com/stable/package_esp8266com_index.json`)
- Required libraries:
  - `DHT sensor library` by Adafruit
  - `Adafruit Unified Sensor`
  - `ESP8266WebServer`
  - `DNSServer` (for captive portal)
  - `LittleFS`

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/[your-username]/SCOUT.git
   cd SCOUT
   ```

2. **Configure your settings**
   ```bash
   cp firmware/config.example.h firmware/config.h
   ```
   Edit `config.h` and set:
   - `CAMERA_SSID` — your E88 camera's AP name
   - `GAS_THRESHOLD` — MQ2 alert level
   - `AIR_THRESHOLD` — MQ135 alert level
   - `BATTERY_LOW` — low voltage cutoff

3. **Flash the firmware**
   - Open `firmware/SCOUT.ino` in Arduino IDE
   - Select board: **NodeMCU 1.0 (ESP-12E Module)**
   - Upload

4. **Connect and control**
   - Power on the bot
   - Join the camera's WiFi AP from your phone
   - Open a browser and navigate to the ESP's IP (shown on serial monitor)
   - The S.C.O.U.T. dashboard loads — drive away! 🚗

---

## 🗺️ Roadmap

| Version | Codename | Status | Milestone |
|---------|----------|--------|-----------|
| v0.1.0 | **Pup** | 🚧 In Progress | Motor control + serial |
| v0.2.0 | **Tracker** | ⏳ Planned | Sensor integration |
| v0.3.0 | **Watcher** | ⏳ Planned | WiFi + web dashboard |
| v0.4.0 | **Ranger** | ⏳ Planned | Autonomous modes |
| v1.0.0 | **Sentinel** | 🎯 Target | Competition-ready |

See [CHANGELOG.md](CHANGELOG.md) for details.

---

## 📁 Project Structure

```
SCOUT/
├── LICENSE
├── NOTICE
├── README.md
├── CHANGELOG.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── WIRING.md
│   ├── API.md
│   └── CALIBRATION.md
├── firmware/
│   ├── SCOUT.ino
│   ├── config.h
│   ├── core/
│   ├── drivers/
│   ├── network/
│   └── web/
└── hardware/
    ├── schematic.pdf
    ├── bom.csv
    └── enclosure/
```

---

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct.

---

## 📜 License

Licensed under the **Apache License, Version 2.0**.
See [LICENSE](LICENSE) for the full text.

```
Copyright 2026 Absolute Tech

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

---

## 🙏 Acknowledgements

- [ESP8266 Arduino Core](https://github.com/esp8266/Arduino)
- [FreeRTOS](https://www.freertos.org/)
- [Adafruit DHT Library](https://github.com/adafruit/DHT-sensor-library)
- All open-source contributors who made this possible

---

## 📬 Contact

**Team Absolute Tech**
- 🏫 The Sapience School , Vikasnager , Dehradun , India
- 📧 aosmicservices@gmail.com
- 🔗 https://github.com/Aosmic-S/S.C.O.U.T./

**Team Members:**
- Arham Ali — Software Engineer
- Devansh Rana - Hardware Engineer 

---

<div align="center">

**⭐ If S.C.O.U.T. helped you, give it a star! ⭐**

*Built with ❤️ by Team Absolute Tech — for safer rescues.*

</div>
