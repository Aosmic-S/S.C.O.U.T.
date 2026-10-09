/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: wifi_sta.h · Purpose: WiFi station connection to camera AP with auto-reconnect interface
 */

#ifndef WIFI_STA_H
#define WIFI_STA_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
public:
    static WiFiManager& instance();

    void init();
    void update();

    bool isConnected() const { return connected_; }
    String getIP() const { return ip_address_; }
    int8_t getRSSI() const { return rssi_; }

private:
    WiFiManager();

    bool connected_ = false;
    String ip_address_ = "0.0.0.0";
    int8_t rssi_ = 0;
    uint32_t last_reconnect_ms_ = 0;
    uint32_t boot_start_ms_ = 0;
};

#endif // WIFI_STA_H
