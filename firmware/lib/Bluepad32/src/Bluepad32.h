/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: Bluepad32.h · Purpose: ControllerPtr typedef and BP32 interface declarations
 */

#ifndef BLUEPAD32_H
#define BLUEPAD32_H

#include <Arduino.h>

#define BP32_MAX_GAMEPADS 4

class Controller;
typedef Controller* ControllerPtr;

class Controller {
public:
  bool isConnected() const { return false; }
  uint8_t battery() const { return 100; }
  int32_t axisY() const { return 0; }
  int32_t axisX() const { return 0; }
  bool a() const { return false; }
  bool b() const { return false; }
  bool x() const { return false; }
  bool y() const { return false; }
  bool miscStart() const { return false; }
  bool miscSelect() const { return false; }
  bool miscHome() const { return false; }
  bool dpadUp() const { return false; }
  bool dpadDown() const { return false; }
  bool dpadLeft() const { return false; }
  bool dpadRight() const { return false; }
  void disconnect() {}
};

class BP32Class {
public:
  void setup(void (*onConnected)(ControllerPtr), void (*onDisconnected)(ControllerPtr)) {
    (void)onConnected;
    (void)onDisconnected;
  }
  void update() {}
  void forgetBluetoothKeys() {}
};

extern BP32Class BP32;

#endif // BLUEPAD32_H
