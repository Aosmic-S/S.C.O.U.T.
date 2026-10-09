/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: headlights_ws2812.cpp · Purpose: WS2812 dual LED headlights driver implementation
 */

#include "headlights_ws2812.h"
#include "../config.h"
#include "../core/state.h"

HeadlightsDriver& HeadlightsDriver::instance() {
    static HeadlightsDriver instance_;
    return instance_;
}

HeadlightsDriver::HeadlightsDriver()
    : pixels_(NUM_HEADLIGHT_LEDS, PIN_HEADLIGHTS_WS2812, NEO_GRB + NEO_KHZ800) {}

void HeadlightsDriver::init() {
    pixels_.begin();
    pixels_.clear();
    pixels_.show();
}

void HeadlightsDriver::setMode(HeadlightMode mode) {
    mode_ = mode;
    StateManager::instance().setHeadlightMode((uint8_t)mode_);
}

void HeadlightsDriver::setBrightness(uint8_t brightness) {
    brightness_ = brightness;
    pixels_.setBrightness(brightness_);
    StateManager::instance().setHeadlightBrightness(brightness_);
}

void HeadlightsDriver::setColor(uint8_t r, uint8_t g, uint8_t b) {
    color_ = pixels_.Color(r, g, b);
    pixels_.setBrightness(brightness_);
}

void HeadlightsDriver::update() {
    uint32_t now = millis();

    switch (mode_) {
        case HeadlightMode::OFF:
            pixels_.clear();
            break;

        case HeadlightMode::LOW_BEAM:
            pixels_.setBrightness(80);
            for (int i = 0; i < NUM_HEADLIGHT_LEDS; i++) {
                pixels_.setPixelColor(i, color_);
            }
            break;

        case HeadlightMode::HIGH_BEAM:
            pixels_.setBrightness(255);
            for (int i = 0; i < NUM_HEADLIGHT_LEDS; i++) {
                pixels_.setPixelColor(i, color_);
            }
            break;

        case HeadlightMode::HAZARD:
            if (now >= hazard_toggle_ms_) {
                hazard_state_ = !hazard_state_;
                hazard_toggle_ms_ = now + 500;
            }
            pixels_.setBrightness(255);
            if (hazard_state_) {
                uint32_t amber = pixels_.Color(255, 120, 0);
                for (int i = 0; i < NUM_HEADLIGHT_LEDS; i++) {
                    pixels_.setPixelColor(i, amber);
                }
            } else {
                pixels_.clear();
            }
            break;

        case HeadlightMode::AUTO: {
            SystemState st = StateManager::instance().getState();
            if (st.alert_level == AlertLevel::DANGER || st.alert_level == AlertLevel::CRITICAL) {
                // Flash hazard amber on high alert
                if (now >= hazard_toggle_ms_) {
                    hazard_state_ = !hazard_state_;
                    hazard_toggle_ms_ = now + 300;
                }
                pixels_.setBrightness(255);
                if (hazard_state_) {
                    uint32_t red_orange = pixels_.Color(255, 40, 0);
                    for (int i = 0; i < NUM_HEADLIGHT_LEDS; i++) {
                        pixels_.setPixelColor(i, red_orange);
                    }
                } else {
                    pixels_.clear();
                }
            } else {
                // High beam white
                pixels_.setBrightness(200);
                for (int i = 0; i < NUM_HEADLIGHT_LEDS; i++) {
                    pixels_.setPixelColor(i, color_);
                }
            }
            break;
        }
    }

    pixels_.show();
}
