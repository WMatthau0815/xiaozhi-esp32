#pragma once
#include "lvgl.h"
#include "display.h"
//#include "temperature_sensor.h"   // NEU

#define TEMP_LABEL_Y 110   // for GC9A01 it's 70 - absolute Y-Position von oben, unabhängig von Uhr-Mittelpunkt

class AnalogClock {
public:
//    static void Start(lv_obj_t* parent, Display* display, TemperatureSensor* temp_sensor);  // GEÄNDERT
    static void Start(lv_obj_t* parent, Display* display);  // GEÄNDERT
    static void Stop(Display* display);
private:
    static void TimerCb(lv_timer_t* timer);
    static void UpdateHands();
//    static void UpdateTemperature();   // NEU
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
//    static lv_obj_t* temp_label_;        // NEU
    static lv_obj_t* clock_container_;
    static lv_timer_t* clock_timer_;
    static Display* display_;
//    static TemperatureSensor* temp_sensor_;   // NEU
    static lv_point_precise_t sec_pts_[2];
    static lv_point_precise_t sec_cw_pts_[2];
    static lv_point_precise_t min_pts_[2];
    static lv_point_precise_t hr_pts_[2];
};
