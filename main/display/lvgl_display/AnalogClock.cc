// main/display/lvgl_display/analog_clock.cc
#include "AnalogClock.h"
#include <cmath>
#include <ctime>
#include "esp_log.h"

static const char* TAG = "AnalogClock";

// --- Statische Member initialisieren ---
lv_obj_t*  AnalogClock::sec_hand_       = nullptr;
lv_obj_t*  AnalogClock::sec_cw_         = nullptr;
lv_obj_t*  AnalogClock::min_hand_       = nullptr;
lv_obj_t*  AnalogClock::hr_hand_        = nullptr;
lv_obj_t*  AnalogClock::center_dot_     = nullptr;
lv_obj_t*  AnalogClock::clock_container_ = nullptr;
lv_timer_t* AnalogClock::clock_timer_   = nullptr;
int         AnalogClock::cx_            = 120;
int         AnalogClock::cy_            = 120;

lv_point_precise_t AnalogClock::sec_pts_[2]    = {};
lv_point_precise_t AnalogClock::sec_cw_pts_[2] = {};
lv_point_precise_t AnalogClock::min_pts_[2]    = {};
lv_point_precise_t AnalogClock::hr_pts_[2]     = {};

// --- Hilfsfunktionen ---
void AnalogClock::CalcHand(float deg, int length, lv_point_precise_t* pts) {
    float angle = (deg - 90.0f) * 0.0174532925f;
    pts[0].x = cx_;
    pts[0].y = cy_;
    pts[1].x = cx_ + (int)(cosf(angle) * length);
    pts[1].y = cy_ + (int)(sinf(angle) * length);
}

lv_obj_t* AnalogClock::CreateHand(lv_obj_t* parent, lv_point_precise_t* pts,
                                    uint32_t color, int width) {
    lv_obj_t* line = lv_line_create(parent);
    lv_line_set_points(line, pts, 2);
    lv_obj_set_style_line_color(line, lv_color_hex(color), 0);
    lv_obj_set_style_line_width(line, width, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    return line;
}

// --- Zifferblatt zeichnen ---
void AnalogClock::DrawFace(lv_obj_t* parent) {
    // Container als Wurzel für alle Uhren-Objekte
    clock_container_ = lv_obj_create(parent);
    lv_obj_set_size(clock_container_,
                    lv_display_get_horizontal_resolution(lv_display_get_default()),
                    lv_display_get_vertical_resolution(lv_display_get_default()));
    lv_obj_set_pos(clock_container_, 0, 0);
    lv_obj_set_style_bg_color(clock_container_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(clock_container_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(clock_container_, 0, 0);
    lv_obj_set_style_pad_all(clock_container_, 0, 0);
    lv_obj_clear_flag(clock_container_, LV_OBJ_FLAG_SCROLLABLE);

    // Displaymitte ermitteln
    cx_ = lv_display_get_horizontal_resolution(lv_display_get_default()) / 2;
    cy_ = lv_display_get_vertical_resolution(lv_display_get_default()) / 2;

    // Äußerer weißer Ring
    lv_obj_t* ring_w = lv_arc_create(clock_container_);
    lv_obj_set_size(ring_w, 236, 236);
    lv_arc_set_bg_angles(ring_w, 0, 360);
    lv_obj_set_style_arc_color(ring_w, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ring_w, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(ring_w, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(ring_w, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ring_w, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_border_width(ring_w, 0, 0);
    lv_obj_center(ring_w);

    // Roter Ring innen
    lv_obj_t* ring_r = lv_arc_create(clock_container_);
    lv_obj_set_size(ring_r, 222, 222);
    lv_arc_set_bg_angles(ring_r, 0, 360);
    lv_obj_set_style_arc_color(ring_r,
