/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: buzzer.h · Purpose: Active buzzer audio alert driver interface
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class BuzzerDriver {
public:
    static BuzzerDriver& instance();

    void init();
    void update();
    void beep(uint32_t duration_ms = 100);
    void alarm(uint32_t duration_ms = 1000);
    void silence();

private:
    BuzzerDriver();

    bool active_ = false;
    uint32_t stop_time_ms_ = 0;
    uint32_t pattern_toggle_ms_ = 0;
    bool pattern_state_ = false;
    bool is_alarm_pattern_ = false;
};

#endif // BUZZER_H
