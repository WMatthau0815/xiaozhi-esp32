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
Display*    AnalogClock::display_       = nullptr;   // <<< DIESE ZEILE NEU EINFÜGEN
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
//    lv_obj_set_style_bg_opa(clock_container_, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(clock_container_, LV_OPA_TRANSP, 0);
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
    lv_obj_set_style_arc_color(ring_r, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ring_r, 8, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(ring_r, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(ring_r, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ring_r, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_border_width(ring_r, 0, 0);
    lv_obj_center(ring_r);

    // 60 Punkte
    for (int i = 0; i < 60; i++) {
        float angle = (i * 6.0f - 90.0f) * 0.0174532925f;
        int px = cx_ + (int)(cosf(angle) * 102);
        int py = cy_ + (int)(sinf(angle) * 102);
        int ds = (i % 5 == 0) ? 5 : 2;

        lv_obj_t* dot = lv_obj_create(clock_container_);
        lv_obj_set_size(dot, ds, ds);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dot, 0, 0);
        lv_obj_set_style_pad_all(dot, 0, 0);
        lv_obj_clear_flag(dot, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(dot, px - ds / 2, py - ds / 2);
    }

    // Zeiger anlegen
    CalcHand(0, 90, sec_pts_);
    CalcHand(180, 20, sec_cw_pts_);
    CalcHand(0, 84, min_pts_);
    CalcHand(0, 62, hr_pts_);

    sec_hand_ = CreateHand(clock_container_, sec_pts_,    0xFF0000, 2);
    sec_cw_   = CreateHand(clock_container_, sec_cw_pts_, 0xFF0000, 2);
    min_hand_ = CreateHand(clock_container_, min_pts_,    0xFFFFFF, 2);
    hr_hand_  = CreateHand(clock_container_, hr_pts_,     0xFFFFFF, 3);

    // Mittelpunkt
    center_dot_ = lv_obj_create(clock_container_);
    lv_obj_set_size(center_dot_, 7, 7);
    lv_obj_set_style_radius(center_dot_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(center_dot_, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_bg_opa(center_dot_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(center_dot_, 0, 0);
    lv_obj_set_style_pad_all(center_dot_, 0, 0);
    lv_obj_set_pos(center_dot_, cx_ - 3, cy_ - 3);
}

// --- Zeiger updaten ---
void AnalogClock::UpdateHands() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);

    if (t->tm_year < 100) {
        // Zeit noch nicht synchronisiert
        ESP_LOGW(TAG, "Zeit noch nicht synchronisiert");
        return;
    }

    float sdeg = t->tm_sec  * 6.0f;
    float mdeg = t->tm_min  * 6.0f + sdeg * 0.01666667f;
    float hdeg = (t->tm_hour % 12) * 30.0f + mdeg * 0.0833333f;

    CalcHand(sdeg,           90, sec_pts_);
    CalcHand(sdeg + 180.0f,  20, sec_cw_pts_);
    CalcHand(mdeg,           84, min_pts_);
    CalcHand(hdeg,           62, hr_pts_);

    lv_line_set_points(sec_hand_, sec_pts_,    2);
    lv_line_set_points(sec_cw_,   sec_cw_pts_, 2);
    lv_line_set_points(min_hand_, min_pts_,    2);
    lv_line_set_points(hr_hand_,  hr_pts_,     2);

    lv_obj_move_foreground(center_dot_);
}

// --- Public: Start ---
void AnalogClock::Start(lv_obj_t* parent, Display* display) {
    if (clock_container_ != nullptr) {
        ESP_LOGW(TAG, "AnalogClock already running");
        return;
    }
    display_ = display;
    DisplayLockGuard lock(display_);
    ESP_LOGI(TAG, "AnalogClock Start");
    DrawFace(parent);
    UpdateHands();
    clock_timer_ = lv_timer_create(TimerCb, 1000, nullptr);
}

// --- Public: Stop ---
void AnalogClock::Stop(Display* display) {
    DisplayLockGuard lock(display);
    ESP_LOGI(TAG, "AnalogClock Stop");
    if (clock_timer_) {
        lv_timer_del(clock_timer_);
        clock_timer_ = nullptr;
    }
    if (clock_container_) {
        lv_obj_del(clock_container_);
        clock_container_ = nullptr;
    }
    sec_hand_ = sec_cw_ = min_hand_ = hr_hand_ = center_dot_ = nullptr;
    display_ = nullptr;
}

// --- Timer Callback ---
void AnalogClock::TimerCb(lv_timer_t* timer) {
    if (!display_) return;
    DisplayLockGuard lock(display_);
    UpdateHands();
}
