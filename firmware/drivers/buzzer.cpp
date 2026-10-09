/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: buzzer.cpp · Purpose: Active buzzer audio alert driver implementation
 */

#include "buzzer.h"
#include "../config.h"

BuzzerDriver& BuzzerDriver::instance() {
    static BuzzerDriver instance_;
    return instance_;
}

BuzzerDriver::BuzzerDriver() {}

void BuzzerDriver::init() {
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
}

void BuzzerDriver::beep(uint32_t duration_ms) {
    active_ = true;
    is_alarm_pattern_ = false;
    stop_time_ms_ = millis() + duration_ms;
    digitalWrite(PIN_BUZZER, HIGH);
}

void BuzzerDriver::alarm(uint32_t duration_ms) {
    active_ = true;
    is_alarm_pattern_ = true;
    stop_time_ms_ = millis() + duration_ms;
    pattern_toggle_ms_ = millis() + 100;
    pattern_state_ = true;
    digitalWrite(PIN_BUZZER, HIGH);
}

void BuzzerDriver::silence() {
    active_ = false;
    is_alarm_pattern_ = false;
    digitalWrite(PIN_BUZZER, LOW);
}

void BuzzerDriver::update() {
    if (!active_) return;

    uint32_t now = millis();
    if (now >= stop_time_ms_) {
        silence();
        return;
    }

    if (is_alarm_pattern_) {
        if (now >= pattern_toggle_ms_) {
            pattern_state_ = !pattern_state_;
            digitalWrite(PIN_BUZZER, pattern_state_ ? HIGH : LOW);
            pattern_toggle_ms_ = now + 100;
        }
    }
}
