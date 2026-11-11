#include "UI.hpp"
#include "core/lv_obj.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_scroll.h"
#include "core/lv_obj_style.h"
#include "core/lv_obj_style_gen.h"
#include "display/lv_display.h"
#include "esp_log.h"
#include "font/lv_font.h"
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
  lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);
  lv_obj_clear_flag(tileview, LV_OBJ_FLAG_SCROLL_ELASTIC);

  lv_obj_t* telemetryWindow =
      lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);
  lv_obj_set_scrollbar_mode(telemetryWindow, LV_SCROLLBAR_MODE_OFF);
  lv_obj_clear_flag(telemetryWindow, LV_OBJ_FLAG_SCROLLABLE);
  // lv_obj_t* playerWindow = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_LEFT);
  lv_obj_set_style_bg_color(telemetryWindow, lv_color_hex(0x000000),
      LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(telemetryWindow, LV_OPA_COVER, LV_STATE_DEFAULT);

  lv_obj_t* top_bar = lv_obj_create(telemetryWindow);
  lv_obj_set_size(top_bar, lv_pct(100), lv_pct(50));
  lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_opa(top_bar, LV_OPA_TRANSP, LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(top_bar, 0, LV_STATE_DEFAULT);
  lv_obj_set_style_radius(top_bar, 0, LV_STATE_DEFAULT);
  lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t* brake_label = lv_label_create(top_bar);
  lv_label_set_text(brake_label, "BRAKE");
  lv_obj_set_style_text_color(brake_label, lv_color_hex(0x888888), 0);
  lv_obj_align(brake_label, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t* brake_bar = lv_bar_create(top_bar);
  lv_obj_set_size(brake_bar, 125, 15); // Thin 4px height
  lv_obj_align_to(brake_bar, brake_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);
  lv_bar_set_range(brake_bar, 0, 150); // Assuming 1.50G is max
  lv_bar_set_value(brake_bar, 100, LV_ANIM_OFF);

  lv_obj_t* brake_val = lv_label_create(top_bar);
  lv_label_set_text(brake_val, "0.00 G");
  lv_obj_set_style_text_color(brake_val, lv_color_hex(0x93BFCF), 0);
  lv_obj_align_to(brake_val, brake_bar, LV_ALIGN_BOTTOM_LEFT, 0, 30);

  lv_obj_set_style_bg_color(brake_bar, lv_color_hex(0x808080),
      LV_PART_MAIN); // Dark background track
  lv_obj_set_style_bg_color(brake_bar, lv_color_hex(0x93BFCF),
      LV_PART_INDICATOR); // Light blue fill
  lv_obj_set_style_radius(brake_bar, 2, LV_PART_MAIN);
  lv_obj_set_style_radius(brake_bar, 2, LV_PART_INDICATOR);

  lv_obj_t* corner_label = lv_label_create(top_bar);
  lv_label_set_text(corner_label, "CORNER");
  lv_obj_set_style_text_color(corner_label, lv_color_hex(0x888888), 0);
  lv_obj_align(corner_label, LV_ALIGN_TOP_RIGHT, -10, 0);

  lv_obj_t* corner_bar = lv_bar_create(top_bar);
  lv_obj_set_size(corner_bar, 125, 15);
  lv_obj_align_to(corner_bar, corner_label, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 8);
  lv_bar_set_range(corner_bar, 0, 150);
  lv_bar_set_value(corner_bar, 46, LV_ANIM_OFF);

  lv_obj_t* corner_val = lv_label_create(top_bar);
  lv_label_set_text(corner_val, "0.46 G");
  lv_obj_set_style_text_color(corner_val, lv_color_hex(0x93BFCF), 0);
  lv_obj_align_to(corner_val, corner_bar, LV_ALIGN_TOP_RIGHT, -10, 30);

  lv_obj_set_style_bg_color(corner_bar, lv_color_hex(0x808080), LV_PART_MAIN);
  lv_obj_set_style_bg_color(corner_bar, lv_color_hex(0x8B3A3A),
      LV_PART_INDICATOR); // Muted red from the image
  lv_obj_set_style_radius(corner_bar, 2, LV_PART_MAIN);
  lv_obj_set_style_radius(corner_bar, 2, LV_PART_INDICATOR);

  lv_obj_t* arc = lv_arc_create(telemetryWindow);
  lv_obj_set_size(arc, 280, 280);
  lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);
  lv_arc_set_range(arc, -60, 60);
  lv_arc_set_bg_angles(arc, 0, 360);
  lv_obj_align(arc, LV_ALIGN_CENTER, 0, 125);
  lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
  lv_arc_set_value(arc, 16);

  lv_obj_set_style_arc_color(arc, lv_color_hex(0x808080), LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc, 15, LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x93BFCF), LV_PART_INDICATOR);
  lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(arc, lv_color_hex(0x000000), LV_PART_KNOB);
  lv_obj_set_style_pad_all(arc, 3, LV_PART_KNOB);
  lv_obj_set_style_border_color(arc, lv_color_hex(0x93BFCF), LV_PART_KNOB);
  lv_obj_set_style_border_width(arc, 2, LV_PART_KNOB);

  lv_obj_t* leanAngle = lv_label_create(telemetryWindow);
  lv_label_set_text(leanAngle, "16.6°");
  lv_obj_set_style_text_color(leanAngle, lv_color_hex(0xECE3CE), 0);
  lv_obj_set_style_text_font(leanAngle, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_align(leanAngle, LV_ALIGN_CENTER, 0, 60);

  lv_obj_t* leanDir = lv_label_create(telemetryWindow);
  lv_label_set_text(leanDir, "R");
  lv_obj_set_style_text_color(leanDir, lv_color_hex(0x888888), 0);
  lv_obj_align(leanDir, LV_ALIGN_CENTER, 0, 75);
}
