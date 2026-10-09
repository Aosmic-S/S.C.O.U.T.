/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: sensor_mq135.h · Purpose: MQ135 air quality gas sensor driver interface
 */

#ifndef SENSOR_MQ135_H
#define SENSOR_MQ135_H

#include <Arduino.h>

class MQ135Sensor {
public:
    static MQ135Sensor& instance();

    void init();
    void update();
    void calibrate(float ro_kohm = 0.0f);

    uint16_t getRaw() const { return raw_adc_; }
    float getVoltage() const { return voltage_; }
    float getRsRoRatio() const { return rs_ro_ratio_; }
    uint32_t getPPM() const { return ppm_; }
    float getRo() const { return ro_; }

private:
    MQ135Sensor();

    uint16_t raw_adc_ = 0;
    float voltage_ = 0.0f;
    float rs_ro_ratio_ = 0.0f;
    uint32_t ppm_ = 0;
    float ro_ = 10.25f;
};

#endif // SENSOR_MQ135_H
