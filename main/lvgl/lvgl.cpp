#include "lvgl.hpp"
#include "UI/UI.hpp"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "misc/lv_timer.h"
#include "types.hpp"

void lvglTickCounter(void* arg) {
  (void)arg;
  lv_tick_inc(10); // Tick counter 10ms
}

void lvglTask(void* arg) {
  initTelemetryUI();

  ESP_LOGI("lvglTask:", "LVGL task started");
  vTaskDelay(pdMS_TO_TICKS(500));
  {
    MotorcycleTelemetry recievedTelemetry;
    char buffer[64];

    while (true) {
      if (telemetryQueue != NULL &&
          xQueueReceive(telemetryQueue, &recievedTelemetry,
              pdMS_TO_TICKS(20)) == pdTRUE)

        snprintf(buffer, sizeof(buffer), "Lean Angle: %.1f",
            recievedTelemetry.leanAngle);
      lv_label_set_text(lean, buffer);

      snprintf(buffer, sizeof(buffer), "Brake Force: %.1f N",
          recievedTelemetry.breakingForce);
      lv_label_set_text(breaking, buffer);

      snprintf(buffer, sizeof(buffer), "Corner Force: %.1f N",
          recievedTelemetry.corneringForce);
      lv_label_set_text(corner, buffer);

      snprintf(buffer, sizeof(buffer), "Timestamp: %lu ms",
          (unsigned long)recievedTelemetry.timestamp);
      lv_label_set_text(latency, buffer);
      lv_timer_handler();
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    lv_timer_handler();
  }
  vTaskDelete(NULL);
}
