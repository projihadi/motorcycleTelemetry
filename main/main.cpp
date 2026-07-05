#include "esp_log.h"
#include "lv_conf_internal.h"
#include "lvgl.hpp"
#include <LovyanGFX.hpp>
#include <stdint.h>

#define BUF_SIZE ((SCREEN_WIDTH * SCREEN_HEIGHT * LV_COLOR_DEPTH / 8) / 10)

static EXT_RAM_BSS_ATTR uint8_t buffer1Data[BUF_SIZE]
    __attribute__((aligned(32)));
static EXT_RAM_BSS_ATTR uint8_t buffer2Data[BUF_SIZE]
    __attribute__((aligned(32)));
// static EXT_RAM_BSS_ATTR uint8_t buffer3Data[BUF_SIZE]
//     __attribute__((aligned(32)));

static lv_draw_buf_t drawBuffer1;
static lv_draw_buf_t drawBuffer2;
// static lv_draw_buf_t drawBuffer3;

LGFX_Custom lcd;
lv_display_t* display;

void flushToDiplay(lv_display_t* display, const lv_area_t* area,
    uint8_t* pixelMap) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  if (lcd.getStartCount() == 0) {
    lcd.waitDMA();
  }

  lcd.startWrite();

  lcd.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t*)pixelMap);

  lcd.endWrite();
  lv_disp_flush_ready(display);
}

extern "C" void app_main(void) {
  ESP_LOGI("Display", "Initializing display system...");

  lcd.init();
  lcd.initDMA();
  lcd.setRotation(1);
  lcd.setColorDepth(LV_COLOR_DEPTH);

  lv_draw_buf_init(&drawBuffer1, SCREEN_WIDTH, SCREEN_HEIGHT / 10,
      LV_COLOR_FORMAT_RGB565, 0, buffer1Data, BUF_SIZE);

  lv_draw_buf_init(&drawBuffer2, SCREEN_WIDTH, SCREEN_HEIGHT / 10,
      LV_COLOR_FORMAT_RGB565, 0, buffer2Data, BUF_SIZE);

  // lv_draw_buf_init(&drawBuffer3, SCREEN_WIDTH, SCREEN_HEIGHT,
  //     LV_COLOR_FORMAT_RGB565, 0, buffer3Data, BUF_SIZE);

  lv_init();

  display = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_flush_cb(display, flushToDiplay);

  lv_display_set_buffers(display, &drawBuffer1, &drawBuffer2, BUF_SIZE,
      LV_DISPLAY_RENDER_MODE_PARTIAL);

  // lv_display_set_3rd_draw_buffer(display, &drawBuffer3);

  const esp_timer_create_args_t lvPeriodicTimerArgs = {.callback =
                                                           &lvglTickCounter,
      .arg = NULL,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "periodic_gui",
      .skip_unhandled_events = true};

  esp_timer_handle_t lvPeriodicTimer;

  ESP_ERROR_CHECK(esp_timer_create(&lvPeriodicTimerArgs, &lvPeriodicTimer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(lvPeriodicTimer,
      10000)); // 10000microseconds = 10millliseconds
  xTaskCreate(lvglTask, "lvglTask", 4096 * 2, NULL, 5, NULL);
}
