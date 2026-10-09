/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: logger.cpp · Purpose: Mission data black-box CSV logger implementation
 */

#include "logger.h"
#include "state.h"
#include "../config.h"

LoggerManager& LoggerManager::instance() {
    static LoggerManager instance_;
    return instance_;
}

LoggerManager::LoggerManager() : initialized_(false), last_log_ms_(0) {}

bool LoggerManager::init() {
    if (initialized_) return true;

    if (!LittleFS.begin(true)) {
        Serial.println("[LOGGER] LittleFS mount failed!");
        return false;
    }

    initialized_ = true;
    ensureHeader();
    return true;
}

void LoggerManager::ensureHeader() {
    if (!initialized_) return;

    if (!LittleFS.exists(LOG_CSV_PATH)) {
        File f = LittleFS.open(LOG_CSV_PATH, "w");
        if (f) {
            f.println("Timestamp_ms,Mode,AlertLevel,MQ2_PPM,MQ135_PPM,AQI,TempInt_C,HumInt_Pct,TempExt_C,HumExt_Pct,Batt_V,Batt_Pct");
            f.close();
        }
    }
}

void LoggerManager::logTelemetry() {
    if (!initialized_) return;

    ensureHeader();
    SystemState st = StateManager::instance().getState();

    if (LittleFS.exists(LOG_CSV_PATH)) {
        File f = LittleFS.open(LOG_CSV_PATH, "r");
        if (f && f.size() > MAX_LOG_SIZE_BYTES) {
            f.close();
            clearLogs();
        } else if (f) {
            f.close();
        }
    }

    File f = LittleFS.open(LOG_CSV_PATH, "a");
    if (f) {
        f.printf("%u,%s,%s,%u,%u,%u,%.1f,%.1f,%.1f,%.1f,%.2f,%u\n",
                 (unsigned int)millis(),
                 StateManager::instance().modeToString(st.mode).c_str(),
                 StateManager::instance().alertLevelToString(st.alert_level).c_str(),
                 (unsigned int)st.mq2.ppm,
                 (unsigned int)st.mq135.ppm,
                 (unsigned int)st.air_quality_index,
                 st.dht_internal.temperature_c,
                 st.dht_internal.humidity_pct,
                 st.dht_external.temperature_c,
                 st.dht_external.humidity_pct,
                 st.battery.voltage,
                 (unsigned int)st.battery.percentage);
        f.close();
    }
}

void LoggerManager::logBrownout(const String& reason) {
    if (!initialized_) return;

    ensureHeader();
    File f = LittleFS.open(LOG_BROWNOUT_PATH, "a");
    if (f) {
        f.printf("[%u] RESET / BROWNOUT: %s\n", (unsigned int)millis(), reason.c_str());
        f.close();
    }
}

String LoggerManager::getLogsCSV() {
    if (!initialized_ || !LittleFS.exists(LOG_CSV_PATH)) {
        return "Timestamp_ms,Mode,AlertLevel,MQ2_PPM,MQ135_PPM,AQI,TempInt_C,HumInt_Pct,TempExt_C,HumExt_Pct,Batt_V,Batt_Pct\n";
    }

    File f = LittleFS.open(LOG_CSV_PATH, "r");
    if (!f) {
        return "";
    }

    String content = f.readString();
    f.close();
    return content;
}

bool LoggerManager::clearLogs() {
    if (!initialized_) return false;

    if (LittleFS.exists(LOG_CSV_PATH)) {
        LittleFS.remove(LOG_CSV_PATH);
    }
    ensureHeader();
    return true;
}
