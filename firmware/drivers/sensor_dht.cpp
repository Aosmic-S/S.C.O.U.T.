/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: sensor_dht.cpp · Purpose: Dual DHT11 temperature and humidity sensor driver implementation
 */

#include "sensor_dht.h"
#include "../config.h"
#include "../core/state.h"

DualDHTSensor& DualDHTSensor::instance() {
    static DualDHTSensor instance_;
    return instance_;
}

DualDHTSensor::DualDHTSensor()
    : dht_internal_(PIN_DHT_INTERNAL, DHT11),
      dht_external_(PIN_DHT_EXTERNAL, DHT11) {}

void DualDHTSensor::init() {
    dht_internal_.begin();
    dht_external_.begin();
    update();
}

void DualDHTSensor::update() {
    float ti = dht_internal_.readTemperature();
    float hi = dht_internal_.readHumidity();
    String status_int = "ok";
    if (isnan(ti) || isnan(hi)) {
        status_int = "error";
        ti = temp_internal_;
        hi = hum_internal_;
    } else {
        temp_internal_ = ti;
        hum_internal_ = hi;
    }
    StateManager::instance().updateDHTInternal(temp_internal_, hum_internal_, status_int);

    float te = dht_external_.readTemperature();
    float he = dht_external_.readHumidity();
    String status_ext = "ok";
    if (isnan(te) || isnan(he)) {
        status_ext = "error";
        te = temp_external_;
        he = hum_external_;
    } else {
        temp_external_ = te;
        hum_external_ = he;
    }
    StateManager::instance().updateDHTExternal(temp_external_, hum_external_, status_ext);
}
