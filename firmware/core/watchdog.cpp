/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: watchdog.cpp · Purpose: Hardware watchdog timer and brownout monitoring implementation
 */

#include "watchdog.h"
#include "state.h"
#include "../config.h"
#include <rom/rtc.h>

RTC_NOINIT_ATTR static uint32_t rtc_brownout_flag;
#define BROWNOUT_MAGIC_NUMBER 0xBAADF00D

WatchdogManager& WatchdogManager::instance() {
    static WatchdogManager instance_;
    return instance_;
}

WatchdogManager::WatchdogManager() : initialized_(false) {}

void WatchdogManager::init() {
    if (initialized_) return;

#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 5
    esp_task_wdt_config_t config = {
        .timeout_ms = WDT_TIMEOUT_SECONDS * 1000,
        .idle_core_mask = (1 << 0) | (1 << 1),
        .trigger_panic = true
    };
    esp_task_wdt_reconfigure(&config);
#else
    esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);
#endif

    initialized_ = true;
    checkBrownoutAndResetReason();
}

void WatchdogManager::subscribeTask() {
    if (initialized_) {
        esp_task_wdt_add(NULL);
    }
}

void WatchdogManager::unsubscribeTask() {
    if (initialized_) {
        esp_task_wdt_delete(NULL);
    }
}

void WatchdogManager::feed() {
    if (initialized_) {
        esp_task_wdt_reset();
    }
}

bool WatchdogManager::checkBrownoutAndResetReason() {
    RESET_REASON reason0 = rtc_get_reset_reason(0);
    RESET_REASON reason1 = rtc_get_reset_reason(1);

    bool brownout_detected = false;

    if (reason0 == RTCWDT_BROWN_OUT_RESET || reason1 == RTCWDT_BROWN_OUT_RESET || rtc_brownout_flag == BROWNOUT_MAGIC_NUMBER) {
        brownout_detected = true;
        Serial.println("[WATCHDOG] Brownout reset detected!");
        rtc_brownout_flag = 0;
    }

    if (reason0 == RTCWDT_RTC_RESET || reason0 == TG0WDT_SYS_RESET || reason0 == TG1WDT_SYS_RESET) {
        StateManager::instance().incrementWatchdogResets();
    }

    return brownout_detected;
}

String WatchdogManager::getResetReasonString() {
    RESET_REASON reason = rtc_get_reset_reason(0);
    switch (reason) {
        case POWERON_RESET: return "Power-on Reset";
        case SW_RESET: return "Software Reset";
        case OWDT_RESET: return "OWDT Reset";
        case DEEPSLEEP_RESET: return "Deep Sleep Reset";
        case SDIO_RESET: return "SDIO Reset";
        case TG0WDT_SYS_RESET: return "Timer Group 0 Watchdog Reset";
        case TG1WDT_SYS_RESET: return "Timer Group 1 Watchdog Reset";
        case RTCWDT_SYS_RESET: return "RTC Watchdog System Reset";
        case EXT_CPU_RESET: return "External CPU Reset";
        case RTCWDT_CPU_RESET: return "RTC Watchdog CPU Reset";
        case RTCWDT_BROWN_OUT_RESET: return "Brownout Reset";
        case RTCWDT_RTC_RESET: return "RTC Watchdog Reset";
        default: return "Unknown Reset";
    }
}
