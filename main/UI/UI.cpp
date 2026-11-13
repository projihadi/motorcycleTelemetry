#include "UI.hpp"
#include <lvgl.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

/* ---------------------------------------------------------------------
 * Concentric lean-angle dashboard (MotoGP style)
 *
 *   outer ring  : lean angle  (-60..+60 deg, fills left/right from top)
 *   inner left  : braking G   (0..1.50 G, fills bottom -> top)
 *   inner right : cornering G (0..1.50 G, fills bottom -> top)
 *   center      : lean angle (top) / acceleration (bottom)
 *
 * Text priority rule:
 *   |lean| > 20 deg  -> lean text is the BIG one
 *   otherwise        -> acceleration text is the BIG one
 * (with a small hysteresis so it doesn't flicker around 20 deg)
 *
 * Fonts: needs LV_FONT_MONTSERRAT_14 and LV_FONT_MONTSERRAT_32 enabled in
 * lv_conf.h. Change FONT_BIG / FONT_SMALL below to suit your config.
 * ------------------------------------------------------------------- */

#define FONT_BIG (&lv_font_montserrat_32)
#define FONT_SMALL (&lv_font_montserrat_14)
#define FONT_CAPTION (&lv_font_montserrat_14)

#define LEAN_BIG_ON_DEG 20.0f  /* lean becomes the big text above this   */
#define LEAN_BIG_OFF_DEG 18.0f /* ...and returns to small below this     */
#define LEAN_MAX_DEG 60
#define G_MAX_X100 150 /* 1.50 G full scale (value is G * 100) */

/* The lean ring is open at the bottom, so the visible shape spans roughly
   -116px (top of ring) to +88px (bottom of inner arcs) around its center.
   Shifting the whole cluster down by 14px balances the top/bottom margins.
   Tweak this value if you change arc sizes. */
#define UI_CENTER_Y 14

#define COL_BG 0x0B0D12
#define COL_TRACK 0x1E222C
#define COL_TEXT_DIM 0x7B8190
#define COL_TEXT_MAIN 0xF4F1EA
#define COL_LEAN_NORMAL 0x5EEAD4 /* teal  */
#define COL_LEAN_HIGH 0xFF9F1C   /* amber */
#define COL_BRAKE 0xFF4D5A       /* red   */
#define COL_CORNER 0x7CC4FF      /* blue  */

/* Widget handles so ui_update() can reach them */
static lv_obj_t* s_lean_arc;
static lv_obj_t* s_brake_arc;
static lv_obj_t* s_corner_arc;
static lv_obj_t* s_lean_val;
static lv_obj_t* s_lean_cap;
static lv_obj_t* s_accel_val;
static lv_obj_t* s_accel_cap;
static bool s_lean_dominant = false;

/* ---------- helpers ---------- */

static void style_plain(lv_obj_t* o) {
  lv_obj_set_style_bg_opa(o, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(o, 0, 0);
  lv_obj_set_style_pad_all(o, 0, 0);
  lv_obj_set_style_radius(o, 0, 0);
  lv_obj_set_style_shadow_width(o, 0, 0);
  lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t* make_arc(lv_obj_t* parent, int size, int width, uint32_t col) {
  lv_obj_t* a = lv_arc_create(parent);
  lv_obj_set_size(a, size, size);
  lv_obj_align(a, LV_ALIGN_CENTER, 0, UI_CENTER_Y);
  lv_obj_remove_flag(a, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(a, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_style_arc_width(a, width, LV_PART_MAIN);
  lv_obj_set_style_arc_color(a, lv_color_hex(COL_TRACK), LV_PART_MAIN);
  lv_obj_set_style_arc_rounded(a, true, LV_PART_MAIN);

  lv_obj_set_style_arc_width(a, width, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(a, lv_color_hex(col), LV_PART_INDICATOR);
  lv_obj_set_style_arc_rounded(a, true, LV_PART_INDICATOR);
  return a;
}

static void set_tenths(char* buf, size_t n, float v, bool with_sign) {
  int t = (int)lroundf(fabsf(v) * 10.0f);
  const char* sign = with_sign ? (v < -0.049f ? "-" : "+") : "";
  snprintf(buf, n, "%s%d.%d", sign, t / 10, t % 10);
}

/* ---------- public update call ----------
 * lean_deg : signed, negative = left lean, positive = right lean
 * brake_g  : braking G (>= 0)
 * corner_g : cornering G (>= 0)
 * accel_g  : signed longitudinal acceleration in G
 */
static void ui_update(float lean_deg, float brake_g, float corner_g, float accel_g) {
  float lean_abs = fabsf(lean_deg);

  /* hysteresis on which text is dominant */
  if (!s_lean_dominant && lean_abs > LEAN_BIG_ON_DEG)
    s_lean_dominant = true;
  else if (s_lean_dominant && lean_abs < LEAN_BIG_OFF_DEG)
    s_lean_dominant = false;

  /* arcs */
  lv_arc_set_value(s_lean_arc, (int)lroundf(lean_deg));
  lv_arc_set_value(s_brake_arc, (int)lroundf(brake_g * 100.0f));
  lv_arc_set_value(s_corner_arc, (int)lroundf(corner_g * 100.0f));

  /* lean accent color: amber when leaning hard */
  uint32_t lean_col = s_lean_dominant ? COL_LEAN_HIGH : COL_LEAN_NORMAL;
  lv_obj_set_style_arc_color(s_lean_arc, lv_color_hex(lean_col), LV_PART_INDICATOR);
  lv_obj_set_style_border_color(s_lean_arc, lv_color_hex(lean_col), LV_PART_KNOB);
  lv_obj_set_style_shadow_color(s_lean_arc, lv_color_hex(lean_col), LV_PART_INDICATOR);

  /* texts */
  char buf[16];
  set_tenths(buf, sizeof(buf), lean_abs, false);
  char lean_txt[20];
  snprintf(lean_txt, sizeof(lean_txt), "%s\xC2\xB0", buf); /* UTF-8 degree */
  lv_label_set_text(s_lean_val, lean_txt);

  const char* side = (lean_deg < -0.05f) ? "L" : (lean_deg > 0.05f ? "R" : "");
  char cap[24];
  snprintf(cap, sizeof(cap), "LEAN  %s", side);
  lv_label_set_text(s_lean_cap, cap);

  set_tenths(buf, sizeof(buf), accel_g, true);
  lv_label_set_text(s_accel_val, buf);

  /* size + emphasis swap */
  lv_obj_set_style_text_font(s_lean_val, s_lean_dominant ? FONT_BIG : FONT_SMALL, 0);
  lv_obj_set_style_text_font(s_accel_val, s_lean_dominant ? FONT_SMALL : FONT_BIG, 0);

  lv_obj_set_style_text_color(s_lean_val,
      lv_color_hex(s_lean_dominant ? COL_LEAN_HIGH : COL_TEXT_DIM), 0);
  lv_obj_set_style_text_color(s_accel_val,
      lv_color_hex(s_lean_dominant ? COL_TEXT_DIM : COL_TEXT_MAIN), 0);
}

/* ---------- demo animation (preview only) ---------- */
static void demo_timer_cb(lv_timer_t* t) {
  (void)t;
  float s = (float)lv_tick_get() / 1000.0f;
  float lean = 48.0f * sinf(s * 0.7f);
  float brake = fmaxf(0.0f, 1.2f * sinf(s * 1.4f + 1.5f));
  float corner = fabsf(lean) / 60.0f * 1.3f;
  float accel = 0.9f * sinf(s * 1.4f);
  ui_update(lean, brake, corner, accel);
}

void initTelemetryUI(void) {
  lv_obj_t* scr = lv_screen_active();
  lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);

  lv_obj_t* tileview = lv_tileview_create(scr);
  lv_obj_set_size(tileview, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_center(tileview);
  lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);
  lv_obj_clear_flag(tileview, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_set_style_bg_color(tileview, lv_color_hex(COL_BG), 0);

  lv_obj_t* telemetryWindow = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);
  lv_obj_t* playerWindow = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_LEFT);
  (void)playerWindow;
  lv_obj_set_scrollbar_mode(telemetryWindow, LV_SCROLLBAR_MODE_OFF);
  lv_obj_clear_flag(telemetryWindow, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(telemetryWindow, lv_color_hex(COL_BG), 0);
  lv_obj_set_style_bg_opa(telemetryWindow, LV_OPA_COVER, 0);

  /* ============ 1. OUTER: lean angle ring ============ */
  /* 240 deg sweep, opening at the bottom. Rotation 150 puts the arc start at
     lower-left; with range -60..60 in SYMMETRICAL mode the fill grows from
     12 o'clock to the left or right. */
  s_lean_arc = make_arc(telemetryWindow, 232, 12, COL_LEAN_NORMAL);
  lv_arc_set_rotation(s_lean_arc, 150);
  lv_arc_set_bg_angles(s_lean_arc, 0, 240);
  lv_arc_set_mode(s_lean_arc, LV_ARC_MODE_SYMMETRICAL);
  lv_arc_set_range(s_lean_arc, -LEAN_MAX_DEG, LEAN_MAX_DEG);
  lv_arc_set_value(s_lean_arc, 0);

  /* soft glow on the lean fill */
  lv_obj_set_style_shadow_width(s_lean_arc, 16, LV_PART_INDICATOR);
  lv_obj_set_style_shadow_color(s_lean_arc, lv_color_hex(COL_LEAN_NORMAL), LV_PART_INDICATOR);
  lv_obj_set_style_shadow_opa(s_lean_arc, LV_OPA_40, LV_PART_INDICATOR);

  /* knob as a small "bike marker" dot */
  lv_obj_set_style_bg_color(s_lean_arc, lv_color_hex(COL_TEXT_MAIN), LV_PART_KNOB);
  lv_obj_set_style_bg_opa(s_lean_arc, LV_OPA_COVER, LV_PART_KNOB);
  lv_obj_set_style_pad_all(s_lean_arc, 2, LV_PART_KNOB);
  lv_obj_set_style_border_width(s_lean_arc, 3, LV_PART_KNOB);
  lv_obj_set_style_border_color(s_lean_arc, lv_color_hex(COL_LEAN_NORMAL), LV_PART_KNOB);

  /* ============ 2. INNER LEFT: braking G ============ */
  /* left half: 95 -> 265 deg (clockwise through 180 = 9 o'clock),
     fills from the bottom upward */
  s_brake_arc = make_arc(telemetryWindow, 176, 14, COL_BRAKE);
  lv_arc_set_bg_angles(s_brake_arc, 95, 265);
  lv_arc_set_mode(s_brake_arc, LV_ARC_MODE_NORMAL);
  lv_arc_set_range(s_brake_arc, 0, G_MAX_X100);
  lv_arc_set_value(s_brake_arc, 0);
  lv_obj_remove_style(s_brake_arc, NULL, LV_PART_KNOB); /* no knob */

  /* ============ 3. INNER RIGHT: cornering G ============ */
  /* right half: 275 -> 445 deg, REVERSE mode so it also fills bottom -> up */
  s_corner_arc = make_arc(telemetryWindow, 176, 14, COL_CORNER);
  lv_arc_set_rotation(s_corner_arc, 275);
  lv_arc_set_bg_angles(s_corner_arc, 0, 170);
  lv_arc_set_mode(s_corner_arc, LV_ARC_MODE_REVERSE);
  lv_arc_set_range(s_corner_arc, 0, G_MAX_X100);
  lv_arc_set_value(s_corner_arc, 0);
  lv_obj_remove_style(s_corner_arc, NULL, LV_PART_KNOB);

  /* ============ 4. CENTER: lean (top) + acceleration (bottom) ======= */
  lv_obj_t* center = lv_obj_create(telemetryWindow);
  style_plain(center);
  lv_obj_set_size(center, 130, 136);
  lv_obj_align(center, LV_ALIGN_CENTER, 0, UI_CENTER_Y);
  lv_obj_set_flex_flow(center, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(center, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(center, 0, 0);

  /* top: lean */
  s_lean_val = lv_label_create(center);
  lv_label_set_text(s_lean_val, "0.0\xC2\xB0");
  lv_obj_set_style_text_font(s_lean_val, FONT_SMALL, 0);
  lv_obj_set_style_text_color(s_lean_val, lv_color_hex(COL_TEXT_DIM), 0);

  s_lean_cap = lv_label_create(center);
  lv_label_set_text(s_lean_cap, "LEAN");
  lv_obj_set_style_text_font(s_lean_cap, FONT_CAPTION, 0);
  lv_obj_set_style_text_color(s_lean_cap, lv_color_hex(COL_TEXT_DIM), 0);
  lv_obj_set_style_text_letter_space(s_lean_cap, 1, 0);

  /* divider */
  lv_obj_t* divider = lv_obj_create(center);
  style_plain(divider);
  lv_obj_set_size(divider, 84, 2);
  lv_obj_set_style_bg_opa(divider, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(divider, lv_color_hex(COL_TRACK), 0);
  lv_obj_set_style_radius(divider, 1, 0);
  lv_obj_set_style_margin_ver(divider, 6, 0);

  /* bottom: acceleration */
  s_accel_val = lv_label_create(center);
  lv_label_set_text(s_accel_val, "+0.0");
  lv_obj_set_style_text_font(s_accel_val, FONT_BIG, 0);
  lv_obj_set_style_text_color(s_accel_val, lv_color_hex(COL_TEXT_MAIN), 0);

  s_accel_cap = lv_label_create(center);
  lv_label_set_text(s_accel_cap, "ACCEL  G");
  lv_obj_set_style_text_font(s_accel_cap, FONT_CAPTION, 0);
  lv_obj_set_style_text_color(s_accel_cap, lv_color_hex(COL_TEXT_DIM), 0);
  lv_obj_set_style_text_letter_space(s_accel_cap, 1, 0);

  /* Demo animation. Remove this line and call ui_update() from your sensor
     loop instead. */
  lv_timer_create(demo_timer_cb, 40, NULL);
}
