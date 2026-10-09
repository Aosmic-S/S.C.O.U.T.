/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: battery_monitor.h · Purpose: Battery voltage divider monitoring driver interface
 */

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>

class BatteryMonitor {
public:
    static BatteryMonitor& instance();

    void init();
    void update();

    float getVoltage() const { return voltage_; }
    uint8_t getPercentage() const { return percentage_; }
    uint32_t getEstimatedRuntimeMinutes() const { return estimated_runtime_min_; }
    bool isLow() const { return is_low_; }

private:
    BatteryMonitor();

    float voltage_ = 12.0f;
    uint8_t percentage_ = 80;
    uint32_t estimated_runtime_min_ = 120;
    bool is_low_ = false;
};

#endif // BATTERY_MONITOR_H
