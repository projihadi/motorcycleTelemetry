#include "lvgl.hpp"
#include "UI/UI.hpp"
#include "esp_log.h"

void lvglTickCounter(void* arg) {
  (void)arg;
  lv_tick_inc(10); // Tick counter 10ms
}

void lvglTask(void* arg) {
  initTelemetryUI();
  {
    ESP_LOGI("lvglTASK", "LVGL task started");

    while (true) {
      lv_timer_handler();
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
  vTaskDelete(NULL);
}
