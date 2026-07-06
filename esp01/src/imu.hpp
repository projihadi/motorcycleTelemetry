#pragma once
#include "driver/i2c.h"
#include "esp_err.h"

static const char* TAG = "IMU";

static esp_err_t initI2C(void);
