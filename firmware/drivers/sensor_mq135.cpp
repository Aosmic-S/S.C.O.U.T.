/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: sensor_mq135.cpp · Purpose: MQ135 air quality gas sensor driver implementation
 */

#include "sensor_mq135.h"
#include "../config.h"
#include "../core/state.h"

MQ135Sensor& MQ135Sensor::instance() {
    static MQ135Sensor instance_;
    return instance_;
}

MQ135Sensor::MQ135Sensor() : ro_(MQ135_RO_DEFAULT) {}

void MQ135Sensor::init() {
    analogSetPinAttenuation(PIN_MQ135_ADC, ADC_11db);
    update();
}

void MQ135Sensor::calibrate(float ro_kohm) {
    if (ro_kohm > 0.1f) {
        ro_ = ro_kohm;
    } else {
        uint32_t sum = 0;
        for (int i = 0; i < 20; i++) {
            sum += analogRead(PIN_MQ135_ADC);
            delayMicroseconds(100);
        }
        float avg_adc = (float)sum / 20.0f;
        float v = (avg_adc / ADC_MAX_VALUE) * ADC_REF_VOLTAGE;
        if (v > 0.1f && v < (ADC_REF_VOLTAGE - 0.1f)) {
            float rs = MQ_RL_KOHM * (ADC_REF_VOLTAGE - v) / v;
            ro_ = rs / 3.6f; // 3.6 is fresh air ratio for MQ135
        }
    }
}

void MQ135Sensor::update() {
    uint32_t sum = 0;
    const uint8_t samples = 10;
    for (uint8_t i = 0; i < samples; i++) {
        sum += analogRead(PIN_MQ135_ADC);
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

    if (ro_ <= 0.01f) ro_ = MQ135_RO_DEFAULT;
    rs_ro_ratio_ = rs / ro_;

    // MQ135 PPM approximation curve: PPM = 116.6 * (Rs/Ro)^-2.769
    if (rs_ro_ratio_ > 0.01f) {
        ppm_ = (uint32_t)(116.6f * pow(rs_ro_ratio_, -2.769f));
    } else {
        ppm_ = 10000;
    }

    String status = "normal";
    if (ppm_ >= MQ135_THRESHOLD_CRIT) {
        status = "critical";
    } else if (ppm_ >= MQ135_THRESHOLD_DANGER) {
        status = "danger";
    } else if (ppm_ >= MQ135_THRESHOLD_WARN) {
        status = "warning";
    }

    StateManager::instance().updateMQ135(raw_adc_, voltage_, rs_ro_ratio_, ppm_, status);
}
