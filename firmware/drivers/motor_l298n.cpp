/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: motor_l298n.cpp · Purpose: L298N dual H-bridge motor driver implementation
 */

#include "motor_l298n.h"
#include "../config.h"
#include "../core/state.h"

MotorDriver& MotorDriver::instance() {
    static MotorDriver instance_;
    return instance_;
}

MotorDriver::MotorDriver() {}

void MotorDriver::init() {
    pinMode(PIN_MOTOR_IN1, OUTPUT);
    pinMode(PIN_MOTOR_IN2, OUTPUT);
    pinMode(PIN_MOTOR_IN3, OUTPUT);
    pinMode(PIN_MOTOR_IN4, OUTPUT);

#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 5
    ledcAttach(PIN_MOTOR_ENA, 5000, 8);
    ledcAttach(PIN_MOTOR_ENB, 5000, 8);
#else
    ledcSetup(0, 5000, 8);
    ledcSetup(1, 5000, 8);
    ledcAttachPin(PIN_MOTOR_ENA, 0);
    ledcAttachPin(PIN_MOTOR_ENB, 1);
#endif

    stop();
}

void MotorDriver::stop() {
    current_dir_ = DriveDirection::STOP;
    current_speed_ = 0;
    applyHardwarePins(0, 0);
}

void MotorDriver::setMotors(DriveDirection dir, uint8_t speed, uint32_t duration_ms) {
    if (StateManager::instance().isEStopActive()) {
        stop();
        return;
    }

    current_dir_ = dir;
    current_speed_ = speed;
    StateManager::instance().updateLastMoveCmdTime();

    if (duration_ms > 0) {
        command_expire_ms_ = millis() + duration_ms;
    } else {
        command_expire_ms_ = 0;
    }

    int16_t left = 0;
    int16_t right = 0;

    switch (dir) {
        case DriveDirection::FORWARD:
            left = speed;
            right = speed;
            break;
        case DriveDirection::BACKWARD:
            left = -speed;
            right = -speed;
            break;
        case DriveDirection::LEFT:
            left = speed / 2;
            right = speed;
            break;
        case DriveDirection::RIGHT:
            left = speed;
            right = speed / 2;
            break;
        case DriveDirection::ROTATE_LEFT:
            left = -speed;
            right = speed;
            break;
        case DriveDirection::ROTATE_RIGHT:
            left = speed;
            right = -speed;
            break;
        case DriveDirection::STOP:
        default:
            left = 0;
            right = 0;
            break;
    }

    applyHardwarePins(left, right);
}

void MotorDriver::setRawPWM(int16_t left_speed, int16_t right_speed) {
    if (StateManager::instance().isEStopActive()) {
        stop();
        return;
    }
    StateManager::instance().updateLastMoveCmdTime();
    applyHardwarePins(left_speed, right_speed);
}

void MotorDriver::applyHardwarePins(int16_t left, int16_t right) {
    // Left motor direction
    if (left > 0) {
        digitalWrite(PIN_MOTOR_IN1, HIGH);
        digitalWrite(PIN_MOTOR_IN2, LOW);
    } else if (left < 0) {
        digitalWrite(PIN_MOTOR_IN1, LOW);
        digitalWrite(PIN_MOTOR_IN2, HIGH);
    } else {
        digitalWrite(PIN_MOTOR_IN1, LOW);
        digitalWrite(PIN_MOTOR_IN2, LOW);
    }

    // Right motor direction
    if (right > 0) {
        digitalWrite(PIN_MOTOR_IN3, HIGH);
        digitalWrite(PIN_MOTOR_IN4, LOW);
    } else if (right < 0) {
        digitalWrite(PIN_MOTOR_IN3, LOW);
        digitalWrite(PIN_MOTOR_IN4, HIGH);
    } else {
        digitalWrite(PIN_MOTOR_IN3, LOW);
        digitalWrite(PIN_MOTOR_IN4, LOW);
    }

    uint8_t left_pwm = abs(left);
    uint8_t right_pwm = abs(right);

#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 5
    ledcWrite(PIN_MOTOR_ENA, left_pwm);
    ledcWrite(PIN_MOTOR_ENB, right_pwm);
#else
    ledcWrite(0, left_pwm);
    ledcWrite(1, right_pwm);
#endif
}

void MotorDriver::update() {
    uint32_t now = millis();

    // 1. E-Stop check
    if (StateManager::instance().isEStopActive()) {
        stop();
        return;
    }

    // 2. Command duration timeout
    if (command_expire_ms_ > 0 && now >= command_expire_ms_) {
        command_expire_ms_ = 0;
        stop();
        return;
    }

    // 3. Dead-man switch (3s without command)
    uint32_t last_cmd = StateManager::instance().getLastMoveCmdTime();
    if (current_dir_ != DriveDirection::STOP && (now - last_cmd > DEADMAN_TIMEOUT_MS)) {
        stop();
        return;
    }

    // 4. Motor stall detection (PWM > 50% for 2s + battery < 10V)
    SystemState st = StateManager::instance().getState();
    if (current_speed_ > 128 && st.battery.voltage > 1.0f && st.battery.voltage < BATTERY_STALL_VOLTS) {
        if (stall_start_ms_ == 0) {
            stall_start_ms_ = now;
        } else if (now - stall_start_ms_ >= STALL_DETECT_MS) {
            Serial.println("[MOTOR] Stall detected (high PWM + low voltage)! Stopping motors.");
            stop();
            return;
        }
    } else {
        stall_start_ms_ = 0;
    }
}

extern "C" void motor_driver_update() {
    MotorDriver::instance().update();
}
