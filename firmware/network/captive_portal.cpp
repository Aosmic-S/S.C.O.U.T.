/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: captive_portal.cpp · Purpose: Captive portal DNS redirect server implementation
 */

#include "captive_portal.h"
#include "../config.h"
#include <WiFi.h>

CaptivePortalManager& CaptivePortalManager::instance() {
    static CaptivePortalManager instance_;
    return instance_;
}

CaptivePortalManager::CaptivePortalManager() {}

void CaptivePortalManager::init() {
    dns_server_.start(DNS_PORT, "*", WiFi.localIP());
    active_ = true;
}

void CaptivePortalManager::update() {
    if (active_) {
        dns_server_.processNextRequest();
    }
}
