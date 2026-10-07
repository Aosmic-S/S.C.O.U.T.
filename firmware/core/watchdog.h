/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: watchdog.h · Purpose: Hardware watchdog timer and brownout monitoring interface
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Arduino.h>
#include <esp_task_wdt.h>

class WatchdogManager {
public:
    static WatchdogManager& instance();

    void init();
    void feed();
    void subscribeTask();
    void unsubscribeTask();

    bool checkBrownoutAndResetReason();
    String getResetReasonString();

private:
    WatchdogManager();
    bool initialized_ = false;
};

#endif // WATCHDOG_H
