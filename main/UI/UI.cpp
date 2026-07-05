#include "UI.hpp"
#include "core/lv_obj_style_gen.h"
#include "esp_log.h"
#include "panelConfig.hpp"

lv_obj_t* lean = nullptr;
lv_obj_t* breaking = nullptr;
lv_obj_t* corner = nullptr;
lv_obj_t* latency = nullptr;

void initTelemetryUI(void) {
  ESP_LOGI("UI", "initTelemetryUI fired");
  lv_obj_t* container = lv_obj_create(lv_screen_active());
  lv_obj_set_size(container, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
      LV_FLEX_ALIGN_CENTER);

  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x0000FF),
      LV_PART_MAIN);
  lean = lv_label_create(container);
  lv_label_set_text(lean, "Lean Angle: --.-");

  breaking = lv_label_create(container);
  lv_label_set_text(breaking, "Lean Angle: --.-");

  corner = lv_label_create(container);
  lv_label_set_text(corner, "Lean Angle: --.-");

  latency = lv_label_create(container);
  lv_label_set_text(latency, "Lean Angle: --.-");
}
