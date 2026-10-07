/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: state.h · Purpose: Global thread-safe system telemetry and state management
 */

#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

enum class OperatingMode {
    MANUAL = 0,
    SNIFFER = 1,
    PERIMETER = 2,
    RTL = 3
};

enum class AlertLevel {
    NORMAL = 0,
    WARNING = 1,
    DANGER = 2,
    CRITICAL = 3
};

struct MQ2Data {
    uint16_t raw = 0;
    float voltage = 0.0f;
    float rs_ro_ratio = 0.0f;
    uint32_t ppm = 0;
    String status = "normal";
};

struct MQ135Data {
    uint16_t raw = 0;
    float voltage = 0.0f;
    float rs_ro_ratio = 0.0f;
    uint32_t ppm = 0;
    String status = "normal";
};

struct DHTData {
    float temperature_c = 0.0f;
    float humidity_pct = 0.0f;
    String status = "ok";
};

struct BatteryData {
    float voltage = 0.0f;
    uint8_t percentage = 0;
    uint32_t estimated_runtime_min = 0;
    bool is_low = false;
};

struct ControllerState {
    bool connected = false;
    uint8_t battery = 0;
    String last_input = "None";
    int32_t axisX = 0;
    int32_t axisY = 0;
};

struct HeadlightsState {
    uint8_t mode = 0; // 0:OFF, 1:LOW, 2:HIGH, 3:HAZARD, 4:AUTO
    uint8_t brightness = 255;
    uint32_t color = 0xFFFFFF;
};

struct SystemState {
    OperatingMode mode = OperatingMode::MANUAL;
    AlertLevel alert_level = AlertLevel::NORMAL;
    uint32_t air_quality_index = 0;

    MQ2Data mq2;
    MQ135Data mq135;
    DHTData dht_internal;
    DHTData dht_external;
    BatteryData battery;
    ControllerState controller;
    HeadlightsState headlights;

    bool emergency_stop = false;
    bool camera_connected = true;
    uint32_t watchdog_resets = 0;
    uint32_t last_move_cmd_ms = 0;
    String control_token = "";
};

class StateManager {
public:
    static StateManager& instance();

    void init();

    SystemState getState();
    void updateMQ2(uint16_t raw, float voltage, float rs_ro, uint32_t ppm, const String& status);
    void updateMQ135(uint16_t raw, float voltage, float rs_ro, uint32_t ppm, const String& status);
    void updateDHTInternal(float temp, float humidity, const String& status);
    void updateDHTExternal(float temp, float humidity, const String& status);
    void updateBattery(float voltage, uint8_t percentage, uint32_t runtime_min, bool is_low);
    void updateController(bool connected, uint8_t batt, const String& last_input, int32_t x, int32_t y);

    void setHeadlightMode(uint8_t mode);
    void setHeadlightBrightness(uint8_t brightness);

    void setMode(OperatingMode mode);
    OperatingMode getMode();

    void setEStop(bool active);
    bool isEStopActive();

    void updateLastMoveCmdTime();
    uint32_t getLastMoveCmdTime();

    void incrementWatchdogResets();
    uint32_t getWatchdogResets();

    String getControlToken();
    void setControlToken(const String& token);

    String modeToString(OperatingMode mode);
    OperatingMode stringToMode(const String& str);

    String alertLevelToString(AlertLevel level);

private:
    StateManager();
    StateManager(const StateManager&) = delete;
    StateManager& operator=(const StateManager&) = delete;

    SemaphoreHandle_t mutex_;
    SystemState state_;

    void calculateAQIAndAlerts();
};

#endif // STATE_H
