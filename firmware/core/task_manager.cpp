/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: task_manager.cpp · Purpose: FreeRTOS task manager implementation
 */

#include "task_manager.h"
#include "state.h"
#include "watchdog.h"
#include "logger.h"
#include "../config.h"

// Weak stubs with C linkage to match extern "C" driver functions
extern "C" {
__attribute__((weak)) void motor_driver_update() {}
__attribute__((weak)) void web_server_update() {}
__attribute__((weak)) void bluetooth_controller_update() {}
__attribute__((weak)) void sensor_drivers_update() {}
__attribute__((weak)) void wifi_manager_update() {}
}

TaskManager& TaskManager::instance() {
    static TaskManager instance_;
    return instance_;
}

TaskManager::TaskManager() {}

void TaskManager::init() {
    StateManager::instance().init();
    WatchdogManager::instance().init();
    LoggerManager::instance().init();
}

void TaskManager::startAllTasks() {
    xTaskCreatePinnedToCore(motorTask, "motor_task", TASK_MOTOR_STACK, NULL, TASK_MOTOR_PRIORITY, &hMotorTask_, 1);
    xTaskCreatePinnedToCore(webTask, "web_task", TASK_WEB_STACK, NULL, TASK_WEB_PRIORITY, &hWebTask_, 0);
    xTaskCreatePinnedToCore(bluetoothTask, "bluetooth_task", TASK_BT_STACK, NULL, TASK_BT_PRIORITY, &hBluetoothTask_, 1);
    xTaskCreatePinnedToCore(sensorTask, "sensor_task", TASK_SENSOR_STACK, NULL, TASK_SENSOR_PRIORITY, &hSensorTask_, 1);
    xTaskCreatePinnedToCore(wifiTask, "wifi_task", TASK_WIFI_STACK, NULL, TASK_WIFI_PRIORITY, &hWifiTask_, 0);
    xTaskCreatePinnedToCore(loggerTask, "logger_task", TASK_LOGGER_STACK, NULL, TASK_LOGGER_PRIORITY, &hLoggerTask_, 0);
}

void TaskManager::motorTask(void* pvParameters) {
    WatchdogManager::instance().subscribeTask();
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(TASK_MOTOR_PERIOD_MS);

    for (;;) {
        motor_driver_update();
        WatchdogManager::instance().feed();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void TaskManager::webTask(void* pvParameters) {
    WatchdogManager::instance().subscribeTask();
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(TASK_WEB_PERIOD_MS);

    for (;;) {
        web_server_update();
        WatchdogManager::instance().feed();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void TaskManager::bluetoothTask(void* pvParameters) {
    WatchdogManager::instance().subscribeTask();
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(TASK_BT_PERIOD_MS);

    for (;;) {
        bluetooth_controller_update();
        WatchdogManager::instance().feed();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void TaskManager::sensorTask(void* pvParameters) {
    WatchdogManager::instance().subscribeTask();
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(TASK_SENSOR_PERIOD_MS);

    for (;;) {
        sensor_drivers_update();
        WatchdogManager::instance().feed();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void TaskManager::wifiTask(void* pvParameters) {
    WatchdogManager::instance().subscribeTask();
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(TASK_WIFI_PERIOD_MS);

    for (;;) {
        wifi_manager_update();
        WatchdogManager::instance().feed();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void TaskManager::loggerTask(void* pvParameters) {
    WatchdogManager::instance().subscribeTask();
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(TASK_LOGGER_PERIOD_MS);

    for (;;) {
        LoggerManager::instance().logTelemetry();
        WatchdogManager::instance().feed();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
