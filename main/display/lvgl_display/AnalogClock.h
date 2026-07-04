#pragma once
#include "lvgl.h"
#include "display.h"    // NEU: für Display, DisplayLockGuard

class AnalogClock {
public:
    static void Start(lv_obj_t* parent, Display* display);   // GEÄNDERT
    static void Stop(Display* display);                       // GEÄNDERT
private:
    static void TimerCb(lv_timer_t* timer);
    static void UpdateHands();
    static void CalcHand(float deg, int length, lv_point_precise_t* pts);
    static lv_obj_t* CreateHand(lv_obj_t* parent, lv_point_precise_t* pts,
                                 uint32_t color, int width);
    static void DrawFace(lv_obj_t* parent);

    static int cx_;
    static int cy_;

    static lv_obj_t* sec_hand_;
    static lv_obj_t* sec_cw_;
    static lv_obj_t* min_hand_;
    static lv_obj_t* hr_hand_;
    static lv_obj_t* center_dot_;
    static lv_obj_t* clock_container_;
    static lv_timer_t* clock_timer_;
    static Display* display_;   // NEU
    static lv_point_precise_t sec_pts_[2];
    static lv_point_precise_t sec_cw_pts_[2];
    static lv_point_precise_t min_pts_[2];
    static lv_point_precise_t hr_pts_[2];
};
