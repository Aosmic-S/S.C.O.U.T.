/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: sensor_dht.h · Purpose: Dual DHT11 temperature and humidity sensor driver interface
 */

#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

#include <Arduino.h>
#include <DHT.h>

class DualDHTSensor {
public:
    static DualDHTSensor& instance();

    void init();
    void update();

    float getInternalTemperature() const { return temp_internal_; }
    float getInternalHumidity() const { return hum_internal_; }
    float getExternalTemperature() const { return temp_external_; }
    float getExternalHumidity() const { return hum_external_; }

private:
    DualDHTSensor();

    DHT dht_internal_;
    DHT dht_external_;

    float temp_internal_ = 25.0f;
    float hum_internal_ = 50.0f;
    float temp_external_ = 28.0f;
    float hum_external_ = 45.0f;
};

#endif // SENSOR_DHT_H
