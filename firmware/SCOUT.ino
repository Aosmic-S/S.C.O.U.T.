/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: SCOUT.ino · Purpose: Main application entry point for S.C.O.U.T.
 */

#include <Arduino.h>
#include "config.h"
#include "core/state.h"
#include "core/watchdog.h"
#include "core/task_manager.h"

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && millis() < 1000);

    Serial.println("\n==================================================");
    Serial.printf("  S.C.O.U.T. Firmware v%s (%s)\n", SCOUT_VERSION, SCOUT_CODENAME);
    Serial.printf("  %s · Team %s\n", SCOUT_COPYRIGHT, SCOUT_TEAM);
    Serial.println("==================================================\n");

    StateManager::instance().init();
    WatchdogManager::instance().init();
    TaskManager::instance().init();

    TaskManager::instance().startAllTasks();
    Serial.println("[SYSTEM] Boot sequence complete. FreeRTOS scheduler active.");
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
