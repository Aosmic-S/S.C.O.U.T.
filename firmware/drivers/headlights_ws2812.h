/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: headlights_ws2812.h · Purpose: WS2812 dual LED headlights driver interface
 */

#ifndef HEADLIGHTS_WS2812_H
#define HEADLIGHTS_WS2812_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum class HeadlightMode {
    OFF = 0,
    LOW_BEAM = 1,
    HIGH_BEAM = 2,
    HAZARD = 3,
    AUTO = 4
};

class HeadlightsDriver {
public:
    static HeadlightsDriver& instance();

    void init();
    void update();

    void setMode(HeadlightMode mode);
    void setBrightness(uint8_t brightness);
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    HeadlightMode getMode() const { return mode_; }
    uint8_t getBrightness() const { return brightness_; }
    uint32_t getColor() const { return color_; }

private:
    HeadlightsDriver();

    Adafruit_NeoPixel pixels_;
    HeadlightMode mode_ = HeadlightMode::OFF;
    uint8_t brightness_ = 255;
    uint32_t color_ = 0xFFFFFF; // Default white
    uint32_t hazard_toggle_ms_ = 0;
    bool hazard_state_ = false;
};

#endif // HEADLIGHTS_WS2812_H
