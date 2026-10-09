/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: Bluepad32.h · Purpose: Local compatibility interface for Bluepad32 Bluetooth Gamepad Host
 */

#ifndef BLUEPAD32_H
#define BLUEPAD32_H

#include <Arduino.h>

#define BP32_MAX_GAMEPADS 4

class Controller {
public:
    Controller() : connected_(false), axisX_(0), axisY_(0), axisRX_(0), axisRY_(0),
                   buttons_(0), dpad_(0), a_(false), b_(false), x_(false), y_(false),
                   miscStart_(false), miscSelect_(false), miscHome_(false), battery_(100) {}

    bool isConnected() const { return connected_; }
    int32_t axisX() const { return axisX_; }
    int32_t axisY() const { return axisY_; }
    int32_t axisRX() const { return axisRX_; }
    int32_t axisRY() const { return axisRY_; }
    uint16_t buttons() const { return buttons_; }
    uint8_t dpad() const { return dpad_; }
    bool a() const { return a_; }
    bool b() const { return b_; }
    bool x() const { return x_; }
    bool y() const { return y_; }
    bool dpadUp() const { return (dpad_ & 0x01) != 0; }
    bool dpadDown() const { return (dpad_ & 0x02) != 0; }
    bool dpadRight() const { return (dpad_ & 0x04) != 0; }
    bool dpadLeft() const { return (dpad_ & 0x08) != 0; }
    bool miscStart() const { return miscStart_; }
    bool miscSelect() const { return miscSelect_; }
    bool miscHome() const { return miscHome_; }
    uint8_t battery() const { return battery_; }

    void setConnected(bool c) { connected_ = c; }
    void setAxisX(int32_t v) { axisX_ = v; }
    void setAxisY(int32_t v) { axisY_ = v; }
    void setAxisRX(int32_t v) { axisRX_ = v; }
    void setAxisRY(int32_t v) { axisRY_ = v; }
    void setA(bool v) { a_ = v; }
    void setB(bool v) { b_ = v; }
    void setX(bool v) { x_ = v; }
    void setY(bool v) { y_ = v; }
    void setMiscStart(bool v) { miscStart_ = v; }
    void setMiscSelect(bool v) { miscSelect_ = v; }
    void setMiscHome(bool v) { miscHome_ = v; }
    void setBattery(uint8_t v) { battery_ = v; }

private:
    bool connected_;
    int32_t axisX_;
    int32_t axisY_;
    int32_t axisRX_;
    int32_t axisRY_;
    uint16_t buttons_;
    uint8_t dpad_;
    bool a_;
    bool b_;
    bool x_;
    bool y_;
    bool miscStart_;
    bool miscSelect_;
    bool miscHome_;
    uint8_t battery_;
};

typedef Controller* ControllerPtr;
typedef Controller* GamepadPtr;

typedef void (*ControllerCallback)(ControllerPtr ctl);

class BP32Class {
public:
    BP32Class() : onConnected_(nullptr), onDisconnected_(nullptr) {}

    void setup(ControllerCallback onConnected, ControllerCallback onDisconnected) {
        onConnected_ = onConnected;
        onDisconnected_ = onDisconnected;
    }
    void update() {}
    void forgetBluetoothKeys() {}

private:
    ControllerCallback onConnected_;
    ControllerCallback onDisconnected_;
};

extern BP32Class BP32;

#endif // BLUEPAD32_H
