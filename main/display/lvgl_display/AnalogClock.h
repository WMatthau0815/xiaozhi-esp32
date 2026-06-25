#pragma once
#include "lvgl.h"

class AnalogClock {
public:
    static void Start(lv_obj_t* parent);
    static void Stop();
private:
    static void TimerCb(lv_timer_t* timer);
    static void UpdateHands();
    static lv_obj_t* sec_hand_;
    static lv_obj_t* sec_cw_;
    static lv_obj_t* min_hand_;
    static lv_obj_t* hr_hand_;
    static lv_obj_t* center_dot_;
    static lv_obj_t* clock_container_;
    static lv_timer_t* clock_timer_;
    static lv_point_precise_t sec_pts_[2];
    static lv_point_precise_t sec_cw_pts_[2];
    static lv_point_precise_t min_pts_[2];
    static lv_point_precise_t hr_pts_[2];
};