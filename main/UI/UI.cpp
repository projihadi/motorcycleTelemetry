#include "UI.hpp"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_style.h"
#include "core/lv_obj_style_gen.h"
#include "display/lv_display.h"
#include "esp_log.h"
#include "misc/lv_area.h"
#include "panelConfig.hpp"
#include "widgets/arc/lv_arc.h"
#include "widgets/tileview/lv_tileview.h"

lv_obj_t* lean = nullptr;
lv_obj_t* breaking = nullptr;
lv_obj_t* corner = nullptr;
lv_obj_t* latency = nullptr;

void initTelemetryUI(void) {
  ESP_LOGI("UI", "initTelemetryUI fired");
  lv_obj_t* tileview = lv_tileview_create(lv_screen_active());
  lv_obj_set_size(tileview, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_center(tileview);

  lv_obj_t* telemetryWindow =
      lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);

  lv_obj_t* playerWindow = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_LEFT);

  lv_obj_set_style_bg_color(telemetryWindow, lv_color_hex(0x000000),
      LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(telemetryWindow, LV_OPA_COVER, LV_STATE_DEFAULT);

  lv_obj_t* arc = lv_arc_create(telemetryWindow);
  lv_obj_set_size(arc, 250, 250);
  lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);
  lv_arc_set_range(arc, -60, 60);
  lv_arc_set_bg_angles(arc, 180, 360);
  lv_obj_align(arc, LV_ALIGN_BOTTOM_MID, 0, 120);
  lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
  lv_arc_set_value(arc, -30);
}
