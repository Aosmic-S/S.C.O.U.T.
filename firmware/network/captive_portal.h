/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: captive_portal.h · Purpose: Captive portal DNS redirect server interface
 */

#ifndef CAPTIVE_PORTAL_H
#define CAPTIVE_PORTAL_H

#include <Arduino.h>
#include <DNSServer.h>

class CaptivePortalManager {
public:
    static CaptivePortalManager& instance();

    void init();
    void update();

private:
    CaptivePortalManager();

    DNSServer dns_server_;
    bool active_ = false;
};

#endif // CAPTIVE_PORTAL_H
