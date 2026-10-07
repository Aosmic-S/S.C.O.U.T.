/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: task_manager.h · Purpose: FreeRTOS task manager interface
 */

#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifdef __cplusplus
extern "C" {
#endif

void motor_driver_update();
void web_server_update();
void bluetooth_controller_update();
void sensor_drivers_update();
void wifi_manager_update();

#ifdef __cplusplus
}
#endif

class TaskManager {
public:
    static TaskManager& instance();

    void init();
    void startAllTasks();

private:
    TaskManager();

    static void motorTask(void* pvParameters);
    static void webTask(void* pvParameters);
    static void bluetoothTask(void* pvParameters);
    static void sensorTask(void* pvParameters);
    static void wifiTask(void* pvParameters);
    static void loggerTask(void* pvParameters);

    TaskHandle_t hMotorTask_ = NULL;
    TaskHandle_t hWebTask_ = NULL;
    TaskHandle_t hBluetoothTask_ = NULL;
    TaskHandle_t hSensorTask_ = NULL;
    TaskHandle_t hWifiTask_ = NULL;
    TaskHandle_t hLoggerTask_ = NULL;
};

#endif // TASK_MANAGER_H
