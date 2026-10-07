/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: state.cpp · Purpose: Thread-safe global system state management implementation
 */

#include "state.h"
#include "../config.h"

StateManager& StateManager::instance() {
    static StateManager instance_;
    return instance_;
}

StateManager::StateManager() {
    mutex_ = xSemaphoreCreateMutex();
}

void StateManager::init() {
    if (mutex_ == NULL) {
        mutex_ = xSemaphoreCreateMutex();
    }
}

SystemState StateManager::getState() {
    SystemState copy;
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        copy = state_;
        xSemaphoreGive(mutex_);
    }
    return copy;
}

void StateManager::updateMQ2(uint16_t raw, float voltage, float rs_ro, uint32_t ppm, const String& status) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.mq2.raw = raw;
        state_.mq2.voltage = voltage;
        state_.mq2.rs_ro_ratio = rs_ro;
        state_.mq2.ppm = ppm;
        state_.mq2.status = status;
        calculateAQIAndAlerts();
        xSemaphoreGive(mutex_);
    }
}

void StateManager::updateMQ135(uint16_t raw, float voltage, float rs_ro, uint32_t ppm, const String& status) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.mq135.raw = raw;
        state_.mq135.voltage = voltage;
        state_.mq135.rs_ro_ratio = rs_ro;
        state_.mq135.ppm = ppm;
        state_.mq135.status = status;
        calculateAQIAndAlerts();
        xSemaphoreGive(mutex_);
    }
}

void StateManager::updateDHTInternal(float temp, float humidity, const String& status) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.dht_internal.temperature_c = temp;
        state_.dht_internal.humidity_pct = humidity;
        state_.dht_internal.status = status;
        xSemaphoreGive(mutex_);
    }
}

void StateManager::updateDHTExternal(float temp, float humidity, const String& status) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.dht_external.temperature_c = temp;
        state_.dht_external.humidity_pct = humidity;
        state_.dht_external.status = status;
        xSemaphoreGive(mutex_);
    }
}

void StateManager::updateBattery(float voltage, uint8_t percentage, uint32_t runtime_min, bool is_low) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.battery.voltage = voltage;
        state_.battery.percentage = percentage;
        state_.battery.estimated_runtime_min = runtime_min;
        state_.battery.is_low = is_low;
        xSemaphoreGive(mutex_);
    }
}

void StateManager::updateController(bool connected, uint8_t batt, const String& last_input, int32_t x, int32_t y) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.controller.connected = connected;
        state_.controller.battery = batt;
        state_.controller.last_input = last_input;
        state_.controller.axisX = x;
        state_.controller.axisY = y;
        xSemaphoreGive(mutex_);
    }
}

void StateManager::setHeadlightMode(uint8_t mode) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.headlights.mode = mode;
        xSemaphoreGive(mutex_);
    }
}

void StateManager::setHeadlightBrightness(uint8_t brightness) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.headlights.brightness = brightness;
        xSemaphoreGive(mutex_);
    }
}

void StateManager::setMode(OperatingMode mode) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.mode = mode;
        xSemaphoreGive(mutex_);
    }
}

OperatingMode StateManager::getMode() {
    OperatingMode mode = OperatingMode::MANUAL;
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        mode = state_.mode;
        xSemaphoreGive(mutex_);
    }
    return mode;
}

void StateManager::setEStop(bool active) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.emergency_stop = active;
        xSemaphoreGive(mutex_);
    }
}

bool StateManager::isEStopActive() {
    bool estop = false;
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        estop = state_.emergency_stop;
        xSemaphoreGive(mutex_);
    }
    return estop;
}

void StateManager::updateLastMoveCmdTime() {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.last_move_cmd_ms = millis();
        xSemaphoreGive(mutex_);
    }
}

uint32_t StateManager::getLastMoveCmdTime() {
    uint32_t t = 0;
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        t = state_.last_move_cmd_ms;
        xSemaphoreGive(mutex_);
    }
    return t;
}

void StateManager::incrementWatchdogResets() {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.watchdog_resets++;
        xSemaphoreGive(mutex_);
    }
}

uint32_t StateManager::getWatchdogResets() {
    uint32_t r = 0;
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        r = state_.watchdog_resets;
        xSemaphoreGive(mutex_);
    }
    return r;
}

String StateManager::getControlToken() {
    String tok = "";
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        tok = state_.control_token;
        xSemaphoreGive(mutex_);
    }
    return tok;
}

void StateManager::setControlToken(const String& token) {
    if (mutex_ != NULL && xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
        state_.control_token = token;
        xSemaphoreGive(mutex_);
    }
}

void StateManager::calculateAQIAndAlerts() {
    uint32_t mq2_ppm = state_.mq2.ppm;
    uint32_t mq135_ppm = state_.mq135.ppm;
    uint32_t max_ppm = (mq2_ppm > mq135_ppm) ? mq2_ppm : mq135_ppm;

    state_.air_quality_index = max_ppm / 4;

    if (mq2_ppm >= MQ2_THRESHOLD_CRIT || mq135_ppm >= MQ135_THRESHOLD_CRIT) {
        state_.alert_level = AlertLevel::CRITICAL;
    } else if (mq2_ppm >= MQ2_THRESHOLD_DANGER || mq135_ppm >= MQ135_THRESHOLD_DANGER) {
        state_.alert_level = AlertLevel::DANGER;
    } else if (mq2_ppm >= MQ2_THRESHOLD_WARN || mq135_ppm >= MQ135_THRESHOLD_WARN) {
        state_.alert_level = AlertLevel::WARNING;
    } else {
        state_.alert_level = AlertLevel::NORMAL;
    }
}

String StateManager::modeToString(OperatingMode mode) {
    switch (mode) {
        case OperatingMode::SNIFFER: return "sniffer";
        case OperatingMode::PERIMETER: return "perimeter";
        case OperatingMode::RTL: return "rtl";
        case OperatingMode::MANUAL:
        default: return "manual";
    }
}

OperatingMode StateManager::stringToMode(const String& str) {
    String lower = str;
    lower.toLowerCase();
    if (lower == "sniffer") return OperatingMode::SNIFFER;
    if (lower == "perimeter") return OperatingMode::PERIMETER;
    if (lower == "rtl" || lower == "return") return OperatingMode::RTL;
    return OperatingMode::MANUAL;
}

String StateManager::alertLevelToString(AlertLevel level) {
    switch (level) {
        case AlertLevel::WARNING: return "warning";
        case AlertLevel::DANGER: return "danger";
        case AlertLevel::CRITICAL: return "critical";
        case AlertLevel::NORMAL:
        default: return "normal";
    }
}
