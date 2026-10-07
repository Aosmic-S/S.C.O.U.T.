/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: ARCHITECTURE.md · Purpose: System architecture documentation
 */

# 🏗️ S.C.O.U.T. System Architecture

## Overview

S.C.O.U.T. operates across 3 distinct architectural layers on the ESP32 LOLIN32 controller:

```
┌────────────────────────────────────────────────────────────────────────┐
│                        USER / GAMEPAD / WEB UI                         │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│ Layer 3: Application & Safety Engine                                  │
│   • REST API Routing & Web Server (ESPAsyncWebServer)                  │
│   • Autonomous Modes (Manual, Sniffer, Perimeter, RTL)                  │
│   • Alert Engine & Multi-Level Hazard Assessor                          │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│ Layer 2: FreeRTOS Real-Time Scheduler                                 │
│   • motor_task (Priority 3, Core 1)     · PWM & Stall Safety           │
│   • bluetooth_task (Priority 2, Core 1) · Gamepad HID Host              │
│   • sensor_task (Priority 2, Core 1)    · ADC1 Sampling & DHT           │
│   • web_task (Priority 2, Core 0)       · Async HTTP & Captive Portal    │
│   • wifi_task (Priority 1, Core 0)      · Station Reconnect             │
│   • logger_task (Priority 1, Core 0)    · LittleFS CSV Blackbox         │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│ Layer 1: Hardware Abstraction & Drivers                               │
│   • motor_l298n      · battery_monitor  · sensor_mq2                  │
│   • sensor_mq135     · sensor_dht       · buzzer                      │
│   • controller_bt    · littlefs_wrapper                               │
└────────────────────────────────────────────────────────────────────────┘
```

## Watchdog & Fail-Safe Architecture

1. **Hardware Watchdog**: 8-second `esp_task_wdt` timer fed in every FreeRTOS task loop.
2. **Dead-Man Switch**: Stops motor output if no command is received for >3000ms.
3. **Stall Detection**: Stops motors if PWM > 50% for 2s while battery voltage drops below 10.0V.
4. **Network Loss Protection**: Immediate motor halt if camera WiFi STA connection drops.
5. **Bluetooth Disconnect Protection**: Stops motors within 100ms on gamepad disconnection.
