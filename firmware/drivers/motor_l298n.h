/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: motor_l298n.h · Purpose: L298N dual H-bridge motor driver interface
 */

#ifndef MOTOR_L298N_H
#define MOTOR_L298N_H

#include <Arduino.h>

enum class DriveDirection {
    STOP = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    ROTATE_LEFT,
    ROTATE_RIGHT
};

class MotorDriver {
public:
    static MotorDriver& instance();

    void init();
    void setMotors(DriveDirection dir, uint8_t speed, uint32_t duration_ms = 0);
    void setRawPWM(int16_t left_speed, int16_t right_speed);
    void stop();
    void update();

    DriveDirection getDirection() const { return current_dir_; }
    uint8_t getSpeed() const { return current_speed_; }

private:
    MotorDriver();

    DriveDirection current_dir_ = DriveDirection::STOP;
    uint8_t current_speed_ = 0;
    uint32_t command_expire_ms_ = 0;
    uint32_t stall_start_ms_ = 0;
    bool stall_detected_ = false;

    void applyHardwarePins(int16_t left, int16_t right);
};

#endif // MOTOR_L298N_H
