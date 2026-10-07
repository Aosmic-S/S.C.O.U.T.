/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: WIRING.md · Purpose: Hardware pin mapping and wiring schematic guide
 */

# 🔌 Hardware Pin Mapping & Wiring Guide

## Core Board: ESP32 LOLIN32 (CP2102)

> ⚠️ **CRITICAL ADC WARNING**: Analog gas sensors and battery monitor MUST be connected exclusively to **ADC1 pins (GPIO 32, 34, 35)**. ADC2 pins cannot be used simultaneously with WiFi enabled.

---

## Pin Assignment Table

| Subsystem | Function | ESP32 GPIO | Description / Notes |
|-----------|----------|------------|---------------------|
| Motor | ENA | GPIO 13 | Left motor PWM speed |
| Motor | IN1 | GPIO 12 | Left motor direction 1 |
| Motor | IN2 | GPIO 14 | Left motor direction 2 |
| Motor | ENB | GPIO 27 | Right motor PWM speed |
| Motor | IN3 | GPIO 26 | Right motor direction 1 |
| Motor | IN4 | GPIO 25 | Right motor direction 2 |
| Analog | MQ2 | GPIO 34 | ADC1 Channel 6 (Smoke/Gas) |
| Analog | MQ135 | GPIO 35 | ADC1 Channel 7 (Air Quality) |
| Analog | Battery | GPIO 32 | ADC1 Channel 4 (Voltage Divider) |
| Digital | DHT11 Internal | GPIO 4 | Chassis internal temp/hum (10kΩ pull-up) |
| Digital | DHT11 External | GPIO 2 | Ambient temp/hum (10kΩ pull-up) |
| Digital | WS2812 Headlights | GPIO 18 | Dual WS2812 RGB Headlights Data Line |
| Output | Buzzer | GPIO 15 | Active Buzzer |
| Output | Status LED | GPIO 5 | Onboard status LED |

---

## Voltage Divider Circuit (Battery Monitor)

- R1 = 100kΩ
- R2 = 22kΩ
- Formula: `V_battery = V_adc * 5.545`
- Attenuation: `ADC_11db`
