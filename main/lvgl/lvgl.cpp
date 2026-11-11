#include "lvgl.hpp"
#include "UI/UI.hpp"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "misc/lv_timer.h"

void lvglTickCounter(void* arg) {
  (void)arg;
  lv_tick_inc(10); // Tick counter 10ms
}

void lvglTask(void* arg) {
  initTelemetryUI();

  ESP_LOGI("lvglTask:", "LVGL task started");
  vTaskDelay(pdMS_TO_TICKS(500));

  while (true) {
    lv_timer_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
