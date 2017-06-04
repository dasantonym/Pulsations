//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_TYPES_H
#define PULSATIONS_TYPES_H

#include "ofMain.h"
#include "ofxOscMessage.h"
#include "Trigger3D.h"

struct sensor_frame_t {
    uint64_t time;
    uint64_t time_received;
    ofVec3f acceleration;
    ofVec3f orientation;
    vector<float> data;
    ofBuffer calibration;
};

struct sensor_status_t {
    bool active;
    ofBuffer calibration;
};

struct sensor_trigger_3d_t {
    string target;
    string name;
    Trigger3D *trigger;
};

struct sensor_settings_t {
    bool active;
    float accelerationThreshold;
};

struct sensor_loop_t {
    bool active;

    uint64_t time_record;
    uint64_t time_start;
    uint64_t duration;

    uint64_t message_index;
    vector<ofxOscMessage> messages;
};

struct sensor_stats_t {
    vector<ofVec4f> accelerationValues;
    vector<ofVec4f> orientationValues;

    uint32_t  bufferTimeMillis = 1000;

    ofVec3f accelerationAvg = { 0.f, 0.f, 0.f };
    ofVec3f accelerationMax = { 0.f, 0.f, 0.f };
    ofVec3f accelerationMin = { 0.f, 0.f, 0.f };

    float accelerationAvgGlobal = 0.f;
    float accelerationMaxGlobal = 0.f;
    float accelerationMinGlobal = 0.f;
};

struct sensor_source_t {
    string id;
    string type;
    string name;
    uint64_t startTime;
    uint64_t lastTime;
    vector<ofPath> paths;
    vector<sensor_frame_t> frames;
    sensor_settings_t settings;
    sensor_stats_t stats;
};

struct remote_command_t {
    uint8_t command;
    bool boolValue;
    float floatValue;
};

class Types {

};


#endif //PULSATIONS_TYPES_H
