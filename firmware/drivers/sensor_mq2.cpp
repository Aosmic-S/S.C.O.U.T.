/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: sensor_mq2.cpp · Purpose: MQ2 gas sensor driver implementation
 */

#include "sensor_mq2.h"
#include "../config.h"
#include "../core/state.h"

MQ2Sensor& MQ2Sensor::instance() {
    static MQ2Sensor instance_;
    return instance_;
}

MQ2Sensor::MQ2Sensor() : ro_(MQ2_RO_DEFAULT) {}

void MQ2Sensor::init() {
    analogSetPinAttenuation(PIN_MQ2_ADC, ADC_11db);
    update();
}

void MQ2Sensor::calibrate(float ro_kohm) {
    if (ro_kohm > 0.1f) {
        ro_ = ro_kohm;
    } else {
        // Read fresh air calibration
        uint32_t sum = 0;
        for (int i = 0; i < 20; i++) {
            sum += analogRead(PIN_MQ2_ADC);
            delayMicroseconds(100);
        }
        float avg_adc = (float)sum / 20.0f;
        float v = (avg_adc / ADC_MAX_VALUE) * ADC_REF_VOLTAGE;
        if (v > 0.1f && v < (ADC_REF_VOLTAGE - 0.1f)) {
            float rs = MQ_RL_KOHM * (ADC_REF_VOLTAGE - v) / v;
            ro_ = rs / 9.83f; // 9.83 is fresh air ratio for MQ2
        }
    }
}

void MQ2Sensor::update() {
    uint32_t sum = 0;
    const uint8_t samples = 10;
    for (uint8_t i = 0; i < samples; i++) {
        sum += analogRead(PIN_MQ2_ADC);
        delayMicroseconds(50);
    }
    raw_adc_ = (uint16_t)(sum / samples);
    voltage_ = (raw_adc_ / ADC_MAX_VALUE) * ADC_REF_VOLTAGE;

    float rs = 0.0f;
    if (voltage_ > 0.05f) {
        rs = MQ_RL_KOHM * (ADC_REF_VOLTAGE - voltage_) / voltage_;
    } else {
        rs = MQ_RL_KOHM * 100.0f;
    }

    if (ro_ <= 0.01f) ro_ = MQ2_RO_DEFAULT;
    rs_ro_ratio_ = rs / ro_;

    // MQ2 PPM approximation log curve: PPM = 613.9 * (Rs/Ro)^-2.074
    if (rs_ro_ratio_ > 0.01f) {
        ppm_ = (uint32_t)(613.9f * pow(rs_ro_ratio_, -2.074f));
    } else {
        ppm_ = 10000;
    }

    String status = "normal";
    if (ppm_ >= MQ2_THRESHOLD_CRIT) {
        status = "critical";
    } else if (ppm_ >= MQ2_THRESHOLD_DANGER) {
        status = "danger";
    } else if (ppm_ >= MQ2_THRESHOLD_WARN) {
        status = "warning";
    }

    StateManager::instance().updateMQ2(raw_adc_, voltage_, rs_ro_ratio_, ppm_, status);
}
