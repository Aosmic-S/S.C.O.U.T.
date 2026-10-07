/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: logger.h · Purpose: Mission data black-box CSV logger interface
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <LittleFS.h>

class LoggerManager {
public:
    static LoggerManager& instance();

    bool init();
    void logTelemetry();
    void logBrownout(const String& reason);
    String getLogsCSV();
    bool clearLogs();

private:
    LoggerManager();
    bool initialized_ = false;
    uint32_t last_log_ms_ = 0;

    void ensureHeader();
};

#endif // LOGGER_H
