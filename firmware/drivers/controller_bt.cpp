/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: controller_bt.cpp · Purpose: Bluepad32 Bluetooth gamepad host driver implementation
 */

#include "controller_bt.h"
#include "../config.h"
#include "../core/state.h"
#include "motor_l298n.h"
#include "buzzer.h"
#include "sensor_mq2.h"
#include "sensor_mq135.h"
#include "sensor_dht.h"
#include "battery_monitor.h"
#include "headlights_ws2812.h"

static ControllerPtr myControllers[BP32_MAX_GAMEPADS];

BluetoothController& BluetoothController::instance() {
    static BluetoothController instance_;
    return instance_;
}

BluetoothController::BluetoothController() {}

void BluetoothController::onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            myControllers[i] = ctl;
            BluetoothController::instance().active_ctl_ = ctl;
            BluetoothController::instance().connected_ = true;
            Serial.printf("[BT] Controller connected at index %d\n", i);
            break;
        }
    }
}

void BluetoothController::onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr;
            if (BluetoothController::instance().active_ctl_ == ctl) {
                BluetoothController::instance().active_ctl_ = nullptr;
                BluetoothController::instance().connected_ = false;
                MotorDriver::instance().stop(); // Auto-stop on disconnect within 100ms
            }
            Serial.printf("[BT] Controller disconnected from index %d\n", i);
            break;
        }
    }
}

void BluetoothController::init() {
    BP32.setup(onConnectedController, onDisconnectedController);
    BP32.forgetBluetoothKeys();
    HeadlightsDriver::instance().init();
}

void BluetoothController::update() {
    BP32.update();

    if (active_ctl_ != nullptr && active_ctl_->isConnected()) {
        connected_ = true;
        battery_ = active_ctl_->battery();
        processControllerInput(active_ctl_);
    } else {
        if (connected_) {
            connected_ = false;
            MotorDriver::instance().stop();
        }
        StateManager::instance().updateController(false, 0, "None", 0, 0);
    }
}

void BluetoothController::processControllerInput(ControllerPtr ctl) {
    uint32_t now = millis();
    int32_t x = ctl->axisX();
    int32_t y = ctl->axisY();

    String input_name = "Analog";

    // Emergency Stop
    if (ctl->y()) {
        StateManager::instance().setEStop(true);
        MotorDriver::instance().stop();
        BuzzerDriver::instance().alarm(500);
        input_name = "Y (E-Stop)";
    }

    // Buzzer control
    if (ctl->a()) {
        BuzzerDriver::instance().beep(100);
        input_name = "A (Beep)";
    }
    if (ctl->b()) {
        BuzzerDriver::instance().alarm(1000);
        input_name = "B (Alarm)";
    }

    // Cycle operating mode
    if (ctl->x() && (now - last_button_time_ > BT_DEBOUNCE_MS * 4)) {
        last_button_time_ = now;
        OperatingMode mode = StateManager::instance().getMode();
        uint8_t next_mode = ((uint8_t)mode + 1) % 4;
        StateManager::instance().setMode((OperatingMode)next_mode);
        input_name = "X (Cycle Mode)";
    }

    // Toggle Manual/Auto with Start+Select
    if (ctl->miscStart() && ctl->miscSelect() && (now - last_button_time_ > BT_DEBOUNCE_MS * 4)) {
        last_button_time_ = now;
        OperatingMode mode = StateManager::instance().getMode();
        if (mode == OperatingMode::MANUAL) {
            StateManager::instance().setMode(OperatingMode::SNIFFER);
        } else {
            StateManager::instance().setMode(OperatingMode::MANUAL);
        }
        input_name = "Start+Select";
    }

    // Return to launch (RTL) with Home
    if (ctl->miscHome() && (now - last_button_time_ > BT_DEBOUNCE_MS * 4)) {
        last_button_time_ = now;
        StateManager::instance().setMode(OperatingMode::RTL);
        input_name = "Home (RTL)";
    }

    // D-Pad Discrete Movements
    if (ctl->dpadUp()) {
        MotorDriver::instance().setMotors(DriveDirection::FORWARD, MOTOR_DEFAULT_SPEED, 250);
        input_name = "D-Pad Up";
    } else if (ctl->dpadDown()) {
        MotorDriver::instance().setMotors(DriveDirection::BACKWARD, MOTOR_DEFAULT_SPEED, 250);
        input_name = "D-Pad Down";
    } else if (ctl->dpadLeft()) {
        MotorDriver::instance().setMotors(DriveDirection::LEFT, MOTOR_DEFAULT_SPEED, 250);
        input_name = "D-Pad Left";
    } else if (ctl->dpadRight()) {
        MotorDriver::instance().setMotors(DriveDirection::RIGHT, MOTOR_DEFAULT_SPEED, 250);
        input_name = "D-Pad Right";
    }
    // Analog Stick Steering
    else if (abs((int)x) > BT_DEADZONE || abs((int)y) > BT_DEADZONE) {
        int16_t left_pwm = -y + x;
        int16_t right_pwm = -y - x;

        left_pwm = constrain(left_pwm, -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);
        right_pwm = constrain(right_pwm, -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);

        MotorDriver::instance().setRawPWM(left_pwm, right_pwm);
    }

    StateManager::instance().updateController(true, battery_, input_name, x, y);
}

extern "C" void sensor_drivers_update() {
    MQ2Sensor::instance().update();
    MQ135Sensor::instance().update();
    DualDHTSensor::instance().update();
    BatteryMonitor::instance().update();
}

extern "C" void bluetooth_controller_update() {
    BluetoothController::instance().update();
    BuzzerDriver::instance().update();
    HeadlightsDriver::instance().update();
}
