/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: wifi_sta.cpp · Purpose: WiFi SoftAP (own hotspot) implementation
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
    WiFi.mode(WIFI_AP);
    IPAddress local_ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    bool ok;
    if (strlen(AP_PASS) >= 8) {
        ok = WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL, 0, AP_MAX_CLIENTS);
    } else {
        ok = WiFi.softAP(AP_SSID, nullptr, AP_CHANNEL, 0, AP_MAX_CLIENTS); // open AP
    }

    connected_ = ok;
    ip_address_ = WiFi.softAPIP().toString();
    boot_start_ms_ = millis();
    Serial.printf("[WIFI] AP %s | SSID: %s | IP: %s\n",
                  ok ? "started" : "FAILED", AP_SSID, ip_address_.c_str());
}

void WiFiManager::update() {
    uint32_t now = millis();

    // Report number of connected clients in rssi_ slot is not meaningful in AP mode
    rssi_ = 0;

    // If the AP ever drops, restart it every WIFI_RECONNECT_MS
    if (!connected_ || WiFi.getMode() != WIFI_AP) {
        if (now - last_reconnect_ms_ >= WIFI_RECONNECT_MS) {
            last_reconnect_ms_ = now;
            Serial.println("[WIFI] AP down, restarting...");
            MotorDriver::instance().stop();
            init();
        }
    }
}

extern "C" void wifi_manager_update() {
    WiFiManager::instance().update();
}
