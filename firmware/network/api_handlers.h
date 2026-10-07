/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: api_handlers.h · Purpose: REST API request handlers interface
 */

#ifndef API_HANDLERS_H
#define API_HANDLERS_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class APIHandlers {
public:
    static void handleStatus(AsyncWebServerRequest *request);
    static void handleSensors(AsyncWebServerRequest *request);
    static void handleBattery(AsyncWebServerRequest *request);
    static void handleLogs(AsyncWebServerRequest *request);
    static void handleLogsCSV(AsyncWebServerRequest *request);
    static void handleMove(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void handleMode(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void handleStop(AsyncWebServerRequest *request);
    static void handleRelease(AsyncWebServerRequest *request);
    static void handleBuzzer(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void handleCalibrate(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void handleHeadlights(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void handleDeleteLogs(AsyncWebServerRequest *request);

    static void sendErrorResponse(AsyncWebServerRequest *request, int code, const String& error, const String& message);
    static bool checkToken(AsyncWebServerRequest *request);
};

#endif // API_HANDLERS_H
