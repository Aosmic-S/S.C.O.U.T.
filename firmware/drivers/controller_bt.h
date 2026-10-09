/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: controller_bt.h · Purpose: Bluepad32 Bluetooth gamepad host driver interface
 */

#ifndef CONTROLLER_BT_H
#define CONTROLLER_BT_H

#include <Arduino.h>
#include "../lib/Bluepad32/src/Bluepad32.h"

class BluetoothController {
public:
    static BluetoothController& instance();

    void init();
    void update();

    bool isConnected() const { return connected_; }
    uint8_t getBattery() const { return battery_; }

private:
    BluetoothController();

    static void onConnectedController(ControllerPtr ctl);
    static void onDisconnectedController(ControllerPtr ctl);

    void processControllerInput(ControllerPtr ctl);

    ControllerPtr active_ctl_ = nullptr;
    bool connected_ = false;
    uint8_t battery_ = 100;
    uint32_t last_input_ms_ = 0;
    uint32_t last_button_time_ = 0;
};

#endif // CONTROLLER_BT_H
