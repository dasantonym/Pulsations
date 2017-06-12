//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_TYPES_H
#define PULSATIONS_TYPES_H

#include "ofMain.h"

struct sensor_frame_t {
    uint64_t time;
    uint64_t time_received;
    ofVec3f acceleration;
    ofVec3f orientation;
    ofVec4f quaternion;
    string sensor_id;
};

struct sensor_status_t {
    bool active;
    ofBuffer calibration;
    ofBuffer system;
};

struct sensor_trigger_t {
    string name;
    int16_t trigger_id;

    string target_type;
    string sensor_id;
};

struct sensor_trigger_result_3f_t {
    sensor_trigger_t trigger;

    bool isTriggered;
    ofVec3f triggerValue;
    ofVec3f debounceStatus;
};

struct remote_command_t {
    uint8_t command;
    bool boolValue;
    float floatValue;
};

struct trigger_time_3d_t {
    int64_t x;
    int64_t y;
    int64_t z;
};

struct trigger_time_4d_t {
    int64_t x;
    int64_t y;
    int64_t z;
    int64_t w;
};

#endif //PULSATIONS_TYPES_H
