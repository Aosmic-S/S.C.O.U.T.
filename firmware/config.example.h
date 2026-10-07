/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: config.example.h · Purpose: Configuration template and system constants
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// SYSTEM IDENTIFICATION
// ============================================================================
#define SCOUT_VERSION       "0.27.0"
#define SCOUT_CODENAME      "OP"
#define SCOUT_TEAM          "Absolute Tech"
#define SCOUT_COPYRIGHT     "Copyright 2026 Absolute Tech"

// Compile-time assertion checks
static_assert(sizeof(SCOUT_VERSION) > 1, "SCOUT_VERSION must be defined");
static_assert(sizeof(SCOUT_CODENAME) > 1, "SCOUT_CODENAME must be defined");

// ============================================================================
// DEBUG & LOGGING
// ============================================================================
#define SCOUT_DEBUG_ENABLE  1
#define SERIAL_BAUD_RATE    115200

// ============================================================================
// HARDWARE PIN MAPPING
// ============================================================================
// Motor Driver (L298N)
#define PIN_MOTOR_ENA       13   // Left motor PWM speed
#define PIN_MOTOR_IN1       12   // Left motor dir 1
#define PIN_MOTOR_IN2       14   // Left motor dir 2
#define PIN_MOTOR_ENB       27   // Right motor PWM speed
#define PIN_MOTOR_IN3       26   // Right motor dir 1
#define PIN_MOTOR_IN4       25   // Right motor dir 2

// Analog Sensors (ADC1 ONLY — ADC2 breaks WiFi)
#define PIN_MQ2_ADC         34   // Smoke / LPG / Methane
#define PIN_MQ135_ADC       35   // Air Quality / CO2 / NH3
#define PIN_BATTERY_ADC     32   // Battery Voltage Divider

// Digital I/O
#define PIN_DHT_INTERNAL    4    // Internal DHT11 (10k pull-up to 3.3V)
#define PIN_DHT_EXTERNAL    2    // External DHT11 (10k pull-up to 3.3V)
#define PIN_BUZZER          15   // Active Buzzer
#define PIN_STATUS_LED      5    // Onboard Status LED
#define PIN_HEADLIGHTS_WS2812 18 // WS2812 Dual Headlights
#define NUM_HEADLIGHT_LEDS  2    // 2 WS2812 LEDs

// ============================================================================
// BATTERY MONITOR SETTINGS
// ============================================================================
#define BATTERY_R1_OHMS     100000.0f  // 100kΩ
#define BATTERY_R2_OHMS     22000.0f   // 22kΩ
#define BATTERY_DIVIDER_RATIO ((BATTERY_R1_OHMS + BATTERY_R2_OHMS) / BATTERY_R2_OHMS) // 5.54545
#define BATTERY_ADC_ATTEN   ADC_11db
#define BATTERY_MIN_VOLTS   9.6f       // 3S Li-ion minimum cutoff
#define BATTERY_MAX_VOLTS   12.6f      // 3S Li-ion fully charged
#define BATTERY_LOW_VOLTS   10.5f      // Low battery warning threshold
#define BATTERY_STALL_VOLTS 10.0f      // Motor stall detection voltage limit

// ============================================================================
// GAS SENSORS & CALIBRATION
// ============================================================================
#define MQ_RL_KOHM          10.0f      // Load resistance in kΩ
#define MQ2_RO_DEFAULT      9.83f      // Default Ro calibration constant for MQ2
#define MQ135_RO_DEFAULT    10.25f     // Default Ro calibration constant for MQ135
#define ADC_MAX_VALUE       4095.0f    // 12-bit ESP32 ADC
#define ADC_REF_VOLTAGE     3.3f       // Reference voltage

// Gas Alert Thresholds (PPM)
#define MQ2_THRESHOLD_WARN  300
#define MQ2_THRESHOLD_DANGER 600
#define MQ2_THRESHOLD_CRIT  1000

#define MQ135_THRESHOLD_WARN 400
#define MQ135_THRESHOLD_DANGER 800
#define MQ135_THRESHOLD_CRIT 1200

// ============================================================================
// NETWORKING & CAPTIVE PORTAL
// ============================================================================
#define CAMERA_SSID         "E88_FPV_DRONE"
#define CAMERA_PASS         ""         // Open WiFi AP
#define WIFI_RECONNECT_MS   5000       // Auto-reconnect interval
#define WIFI_BOOT_TIMEOUT_MS 15000     // Boot timeout for WiFi
#define HTTP_SERVER_PORT    80
#define DNS_PORT            53

// ============================================================================
// MOTOR & SAFETY TIMEOUTS
// ============================================================================
#define MOTOR_DEFAULT_SPEED 180        // PWM 0-255 (approx 70%)
#define MOTOR_MAX_SPEED     255
#define MOTOR_MIN_SPEED     50
#define DEADMAN_TIMEOUT_MS  3000       // Stop motors if no move command in 3s
#define STALL_DETECT_MS     2000       // PWM > 50% for 2s + Batt < 10V => Stop
#define WDT_TIMEOUT_SECONDS 8          // Hardware Watchdog timeout

// ============================================================================
// BLUETOOTH CONTROLLER (CLAW Shoot V3)
// ============================================================================
#define BT_DEADZONE         50         // Stick deadzone (±50 / 512)
#define BT_DEBOUNCE_MS      50         // Button debounce
#define BT_DISCONNECT_STOP_MS 100      // Stop within 100ms on BT loss
#define BT_POLL_INTERVAL_MS 20

// ============================================================================
// FREERTOS TASK CONFIGURATION
// ============================================================================
#define TASK_MOTOR_PRIORITY    3
#define TASK_MOTOR_STACK       4096
#define TASK_MOTOR_PERIOD_MS   20

#define TASK_WEB_PRIORITY      2
#define TASK_WEB_STACK         8192
#define TASK_WEB_PERIOD_MS     50

#define TASK_BT_PRIORITY       2
#define TASK_BT_STACK          4096
#define TASK_BT_PERIOD_MS      20

#define TASK_SENSOR_PRIORITY   2
#define TASK_SENSOR_STACK      4096
#define TASK_SENSOR_PERIOD_MS  500

#define TASK_WIFI_PRIORITY     1
#define TASK_WIFI_STACK        4096
#define TASK_WIFI_PERIOD_MS    1000

#define TASK_LOGGER_PRIORITY   1
#define TASK_LOGGER_STACK      4096
#define TASK_LOGGER_PERIOD_MS  2000

// ============================================================================
// LITTLEFS LOG PATHS
// ============================================================================
#define LOG_CSV_PATH        "/logs/mission.csv"
#define LOG_BROWNOUT_PATH   "/logs/brownout.txt"
#define MAX_LOG_SIZE_BYTES  200000

#endif // CONFIG_H
