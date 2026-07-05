#include "freertos/idf_additions.h"
#include <stdint.h>

#pragma once

typedef struct {
  float leanAngle;
  float breakingForce;
  float corneringForce;
  uint32_t timestamp;
} MotorcycleTelemetry;

extern QueueHandle_t telemetryQueue;

void initQueue(void);
