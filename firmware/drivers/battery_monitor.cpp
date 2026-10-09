/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: battery_monitor.cpp · Purpose: Battery voltage divider monitoring driver implementation
 */

#include "battery_monitor.h"
#include "../config.h"
#include "../core/state.h"

BatteryMonitor& BatteryMonitor::instance() {
    static BatteryMonitor instance_;
    return instance_;
}

BatteryMonitor::BatteryMonitor() {}

void BatteryMonitor::init() {
    analogSetPinAttenuation(PIN_BATTERY_ADC, BATTERY_ADC_ATTEN);
    update();
}

void BatteryMonitor::update() {
    uint32_t adc_sum = 0;
    const uint8_t samples = 10;
    for (uint8_t i = 0; i < samples; i++) {
        adc_sum += analogRead(PIN_BATTERY_ADC);
        delayMicroseconds(50);
    }
    float raw_adc = (float)adc_sum / samples;
    float adc_voltage = (raw_adc / ADC_MAX_VALUE) * ADC_REF_VOLTAGE;
    voltage_ = adc_voltage * BATTERY_DIVIDER_RATIO;

    // Calculate battery percentage
    if (voltage_ >= BATTERY_MAX_VOLTS) {
        percentage_ = 100;
    } else if (voltage_ <= BATTERY_MIN_VOLTS) {
        percentage_ = 0;
    } else {
        percentage_ = (uint8_t)(((voltage_ - BATTERY_MIN_VOLTS) / (BATTERY_MAX_VOLTS - BATTERY_MIN_VOLTS)) * 100.0f);
    }

    is_low_ = (voltage_ <= BATTERY_LOW_VOLTS);

    // Approximate runtime calculation (e.g., 180 min max runtime)
    estimated_runtime_min_ = (uint32_t)((percentage_ / 100.0f) * 180.0f);

    StateManager::instance().updateBattery(voltage_, percentage_, estimated_runtime_min_, is_low_);
}
