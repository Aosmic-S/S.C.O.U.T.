/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: sensor_mq2.h · Purpose: MQ2 gas sensor driver interface
 */

#ifndef SENSOR_MQ2_H
#define SENSOR_MQ2_H

#include <Arduino.h>

class MQ2Sensor {
public:
    static MQ2Sensor& instance();

    void init();
    void update();
    void calibrate(float ro_kohm = 0.0f);

    uint16_t getRaw() const { return raw_adc_; }
    float getVoltage() const { return voltage_; }
    float getRsRoRatio() const { return rs_ro_ratio_; }
    uint32_t getPPM() const { return ppm_; }
    float getRo() const { return ro_; }

private:
    MQ2Sensor();

    uint16_t raw_adc_ = 0;
    float voltage_ = 0.0f;
    float rs_ro_ratio_ = 0.0f;
    uint32_t ppm_ = 0;
    float ro_ = 9.83f;
};

#endif // SENSOR_MQ2_H
