/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: web_server.h · Purpose: ESPAsyncWebServer initialization and routing interface
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class WebServerManager {
public:
    static WebServerManager& instance();

    void init();
    void update();

private:
    WebServerManager();

    AsyncWebServer server_;
    bool initialized_ = false;
};

#endif // WEB_SERVER_H
