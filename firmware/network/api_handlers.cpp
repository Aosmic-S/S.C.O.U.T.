/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: api_handlers.cpp · Purpose: REST API request handlers implementation
 */

#include "api_handlers.h"
#include "../config.h"
#include "../core/state.h"
#include "../core/logger.h"
#include "../core/watchdog.h"
#include "../drivers/motor_l298n.h"
#include "../drivers/buzzer.h"
#include "../drivers/sensor_mq2.h"
#include "../drivers/sensor_mq135.h"
#include "../drivers/headlights_ws2812.h"
#include "wifi_sta.h"
#include <ArduinoJson.h>

void APIHandlers::sendErrorResponse(AsyncWebServerRequest *request, int code, const String& error, const String& message) {
    StaticJsonDocument<256> doc;
    doc["ok"] = false;
    doc["error"] = error;
    doc["message"] = message;
    String response;
    serializeJson(doc, response);
    request->send(code, "application/json", response);
}

bool APIHandlers::checkToken(AsyncWebServerRequest *request) {
    String current_tok = StateManager::instance().getControlToken();
    if (current_tok.length() == 0) return true;

    if (request->hasHeader("X-SCOUT-Token")) {
        const AsyncWebHeader* h = request->getHeader("X-SCOUT-Token");
        if (h->value() == current_tok) return true;
    }
    return false;
}

void APIHandlers::handleStatus(AsyncWebServerRequest *request) {
    SystemState st = StateManager::instance().getState();

    StaticJsonDocument<512> doc;
    doc["ok"] = true;
    doc["version"] = "0.27.0";
    doc["codename"] = "OP";
    doc["team"] = "Absolute Tech";
    doc["uptime_ms"] = millis();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["wifi_rssi"] = WiFiManager::instance().getRSSI();
    doc["ip"] = WiFiManager::instance().getIP();
    doc["mode"] = StateManager::instance().modeToString(st.mode);
    doc["camera_connected"] = st.camera_connected;
    doc["controller_connected"] = st.controller.connected;
    doc["controller_battery"] = st.controller.battery;
    doc["headlights_mode"] = st.headlights.mode;
    doc["watchdog_resets"] = st.watchdog_resets;
    doc["emergency_stop"] = st.emergency_stop;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleSensors(AsyncWebServerRequest *request) {
    SystemState st = StateManager::instance().getState();

    StaticJsonDocument<768> doc;
    JsonObject mq2 = doc.createNestedObject("mq2");
    mq2["raw"] = st.mq2.raw;
    mq2["voltage"] = st.mq2.voltage;
    mq2["rs_ro_ratio"] = st.mq2.rs_ro_ratio;
    mq2["ppm"] = st.mq2.ppm;
    mq2["status"] = st.mq2.status;

    JsonObject mq135 = doc.createNestedObject("mq135");
    mq135["raw"] = st.mq135.raw;
    mq135["voltage"] = st.mq135.voltage;
    mq135["rs_ro_ratio"] = st.mq135.rs_ro_ratio;
    mq135["ppm"] = st.mq135.ppm;
    mq135["status"] = st.mq135.status;

    JsonObject dhti = doc.createNestedObject("dht_internal");
    dhti["temperature_c"] = st.dht_internal.temperature_c;
    dhti["humidity_pct"] = st.dht_internal.humidity_pct;
    dhti["status"] = st.dht_internal.status;

    JsonObject dhte = doc.createNestedObject("dht_external");
    dhte["temperature_c"] = st.dht_external.temperature_c;
    dhte["humidity_pct"] = st.dht_external.humidity_pct;
    dhte["status"] = st.dht_external.status;

    doc["air_quality_index"] = st.air_quality_index;
    doc["alert_level"] = StateManager::instance().alertLevelToString(st.alert_level);
    doc["timestamp_ms"] = millis();

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleBattery(AsyncWebServerRequest *request) {
    SystemState st = StateManager::instance().getState();

    StaticJsonDocument<256> doc;
    doc["voltage"] = st.battery.voltage;
    doc["percentage"] = st.battery.percentage;
    doc["estimated_runtime_min"] = st.battery.estimated_runtime_min;
    doc["is_low"] = st.battery.is_low;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleLogs(AsyncWebServerRequest *request) {
    String csv = LoggerManager::instance().getLogsCSV();
    StaticJsonDocument<256> doc;
    doc["ok"] = true;
    doc["size_bytes"] = csv.length();
    doc["download_url"] = "/api/logs.csv";

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleLogsCSV(AsyncWebServerRequest *request) {
    String csv = LoggerManager::instance().getLogsCSV();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/csv", csv);
    response->addHeader("Content-Disposition", "attachment; filename=\"mission_log.csv\"");
    request->send(response);
}

void APIHandlers::handleMove(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!checkToken(request)) {
        sendErrorResponse(request, 403, "UNAUTHORIZED", "Invalid or missing X-SCOUT-Token header");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) {
        sendErrorResponse(request, 400, "INVALID_JSON", err.c_str());
        return;
    }

    String dir_str = doc["direction"] | "stop";
    uint8_t speed = doc["speed"] | MOTOR_DEFAULT_SPEED;
    uint32_t duration_ms = doc["duration_ms"] | 250;

    DriveDirection dir = DriveDirection::STOP;
    if (dir_str == "forward") dir = DriveDirection::FORWARD;
    else if (dir_str == "backward") dir = DriveDirection::BACKWARD;
    else if (dir_str == "left") dir = DriveDirection::LEFT;
    else if (dir_str == "right") dir = DriveDirection::RIGHT;
    else if (dir_str == "rotate_left") dir = DriveDirection::ROTATE_LEFT;
    else if (dir_str == "rotate_right") dir = DriveDirection::ROTATE_RIGHT;

    MotorDriver::instance().setMotors(dir, speed, duration_ms);

    StaticJsonDocument<128> res;
    res["ok"] = true;
    res["direction"] = dir_str;
    res["speed"] = speed;
    String response;
    serializeJson(res, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleMode(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!checkToken(request)) {
        sendErrorResponse(request, 403, "UNAUTHORIZED", "Invalid or missing X-SCOUT-Token header");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) {
        sendErrorResponse(request, 400, "INVALID_JSON", err.c_str());
        return;
    }

    String mode_str = doc["mode"] | "manual";
    OperatingMode mode = StateManager::instance().stringToMode(mode_str);
    StateManager::instance().setMode(mode);

    StaticJsonDocument<128> res;
    res["ok"] = true;
    res["mode"] = StateManager::instance().modeToString(mode);
    String response;
    serializeJson(res, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleStop(AsyncWebServerRequest *request) {
    StateManager::instance().setEStop(true);
    MotorDriver::instance().stop();
    BuzzerDriver::instance().alarm(300);

    StaticJsonDocument<128> res;
    res["ok"] = true;
    res["emergency_stop"] = true;
    String response;
    serializeJson(res, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleRelease(AsyncWebServerRequest *request) {
    StateManager::instance().setEStop(false);
    BuzzerDriver::instance().beep(100);

    StaticJsonDocument<128> res;
    res["ok"] = true;
    res["emergency_stop"] = false;
    String response;
    serializeJson(res, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleBuzzer(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) {
        sendErrorResponse(request, 400, "INVALID_JSON", err.c_str());
        return;
    }

    String pattern = doc["pattern"] | "beep";
    uint32_t duration_ms = doc["duration_ms"] | 200;

    if (pattern == "alarm") {
        BuzzerDriver::instance().alarm(duration_ms);
    } else if (pattern == "silence") {
        BuzzerDriver::instance().silence();
    } else {
        BuzzerDriver::instance().beep(duration_ms);
    }

    StaticJsonDocument<128> res;
    res["ok"] = true;
    res["pattern"] = pattern;
    String response;
    serializeJson(res, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleCalibrate(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!checkToken(request)) {
        sendErrorResponse(request, 403, "UNAUTHORIZED", "Invalid or missing X-SCOUT-Token header");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) {
        sendErrorResponse(request, 400, "INVALID_JSON", err.c_str());
        return;
    }

    String sensor = doc["sensor"] | "all";
    float ro_kohm = doc["ro_kohm"] | 0.0f;

    if (sensor == "mq2") {
        MQ2Sensor::instance().calibrate(ro_kohm);
    } else if (sensor == "mq135") {
        MQ135Sensor::instance().calibrate(ro_kohm);
    } else {
        MQ2Sensor::instance().calibrate(ro_kohm);
        MQ135Sensor::instance().calibrate(ro_kohm);
    }

    StaticJsonDocument<128> res;
    res["ok"] = true;
    res["calibrated"] = sensor;
    String response;
    serializeJson(res, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleHeadlights(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!checkToken(request)) {
        sendErrorResponse(request, 403, "UNAUTHORIZED", "Invalid or missing X-SCOUT-Token header");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) {
        sendErrorResponse(request, 400, "INVALID_JSON", err.c_str());
        return;
    }

    String mode_str = doc["mode"] | "off";
    uint8_t brightness = doc["brightness"] | 255;

    HeadlightsDriver::instance().setBrightness(brightness);

    if (mode_str == "low") HeadlightsDriver::instance().setMode(HeadlightMode::LOW_BEAM);
    else if (mode_str == "high") HeadlightsDriver::instance().setMode(HeadlightMode::HIGH_BEAM);
    else if (mode_str == "hazard") HeadlightsDriver::instance().setMode(HeadlightMode::HAZARD);
    else if (mode_str == "auto") HeadlightsDriver::instance().setMode(HeadlightMode::AUTO);
    else HeadlightsDriver::instance().setMode(HeadlightMode::OFF);

    StaticJsonDocument<128> res;
    res["ok"] = true;
    res["mode"] = mode_str;
    res["brightness"] = brightness;
    String response;
    serializeJson(res, response);
    request->send(200, "application/json", response);
}

void APIHandlers::handleDeleteLogs(AsyncWebServerRequest *request) {
    if (!checkToken(request)) {
        sendErrorResponse(request, 403, "UNAUTHORIZED", "Invalid or missing X-SCOUT-Token header");
        return;
    }

    bool cleared = LoggerManager::instance().clearLogs();
    StaticJsonDocument<128> res;
    res["ok"] = cleared;
    String response;
    serializeJson(res, response);
    request->send(200, "application/json", response);
}
