#pragma once
#include "panelConfig.hpp"
#include <lvgl.h>

void flushToDiplay(lv_display_t* display, const lv_area_t* area,
    uint8_t* pixelMap);

void lvglTickCounter(void* arg);

void lvglTask(void* arg);
