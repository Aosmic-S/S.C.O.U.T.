/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: wifi_sta.cpp · Purpose: WiFi station connection to camera AP with auto-reconnect implementation
 */

#include "wifi_sta.h"
#include "../config.h"
#include "../drivers/motor_l298n.h"

WiFiManager& WiFiManager::instance() {
    static WiFiManager instance_;
    return instance_;
}

WiFiManager::WiFiManager() {}

void WiFiManager::init() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(CAMERA_SSID, CAMERA_PASS);
    boot_start_ms_ = millis();
}

void WiFiManager::update() {
    uint32_t now = millis();

    if (WiFi.status() == WL_CONNECTED) {
        if (!connected_) {
            connected_ = true;
            ip_address_ = WiFi.localIP().toString();
            Serial.printf("[WIFI] Connected! IP: %s\n", ip_address_.c_str());
        }
        rssi_ = WiFi.RSSI();
    } else {
        if (connected_) {
            connected_ = false;
            ip_address_ = "0.0.0.0";
            rssi_ = 0;
            Serial.println("[WIFI] Connection lost! Stopping motors.");
            MotorDriver::instance().stop(); // Stop motors on WiFi loss
        }

        // Auto-reconnect every 5s
        if (now - last_reconnect_ms_ >= WIFI_RECONNECT_MS) {
            last_reconnect_ms_ = now;
            Serial.println("[WIFI] Attempting auto-reconnect...");
            WiFi.disconnect();
            WiFi.begin(CAMERA_SSID, CAMERA_PASS);
        }
    }
}

extern "C" void wifi_manager_update() {
    WiFiManager::instance().update();
}
