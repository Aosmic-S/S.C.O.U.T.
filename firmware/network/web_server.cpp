/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: web_server.cpp · Purpose: ESPAsyncWebServer initialization and routing implementation
 */

#include "web_server.h"
#include "../config.h"
#include "api_handlers.h"
#include "wifi_sta.h"
#include "captive_portal.h"
#include <LittleFS.h>

WebServerManager& WebServerManager::instance() {
    static WebServerManager instance_;
    return instance_;
}

WebServerManager::WebServerManager() : server_(HTTP_SERVER_PORT), initialized_(false) {}

void WebServerManager::init() {
    if (initialized_) return;

    WiFiManager::instance().init();
    CaptivePortalManager::instance().init();

    // GET endpoints
    server_.on("/api/status", HTTP_GET, APIHandlers::handleStatus);
    server_.on("/api/sensors", HTTP_GET, APIHandlers::handleSensors);
    server_.on("/api/battery", HTTP_GET, APIHandlers::handleBattery);
    server_.on("/api/logs", HTTP_GET, APIHandlers::handleLogs);
    server_.on("/api/logs.csv", HTTP_GET, APIHandlers::handleLogsCSV);

    // POST endpoints (body handlers)
    server_.on("/api/move", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, APIHandlers::handleMove);
    server_.on("/api/mode", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, APIHandlers::handleMode);
    server_.on("/api/stop", HTTP_POST, APIHandlers::handleStop);
    server_.on("/api/release", HTTP_POST, APIHandlers::handleRelease);
    server_.on("/api/buzzer", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, APIHandlers::handleBuzzer);
    server_.on("/api/calibrate", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, APIHandlers::handleCalibrate);
    server_.on("/api/headlights", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, APIHandlers::handleHeadlights);

    // DELETE endpoints
    server_.on("/api/logs", HTTP_DELETE, APIHandlers::handleDeleteLogs);

    // Serve static files from LittleFS with gzip compression enabled
    server_.serveStatic("/", LittleFS, "/web/").setDefaultFile("index.html");

    // Captive portal fallback redirect
    server_.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });

    server_.begin();
    initialized_ = true;
    Serial.println("[WEB] ESPAsyncWebServer started on port 80.");
}

void WebServerManager::update() {
    WiFiManager::instance().update();
    CaptivePortalManager::instance().update();
}

extern "C" void web_server_update() {
    WebServerManager::instance().update();
}
