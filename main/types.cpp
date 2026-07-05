#include "types.hpp"
#include "esp_log.h"
#include <cstddef>
#include <freertos/FreeRTOS.h>
QueueHandle_t telemetryQueue = NULL;
void initQueue(void) {
  telemetryQueue = xQueueCreate(1, sizeof(MotorcycleTelemetry));

  if (telemetryQueue == NULL) {
    ESP_LOGE("telmetryQueue", "Unable to create queue");
  }
}
