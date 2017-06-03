//
// Created by anton on 02/06/17.
//

#ifndef PULSATIONS_SENSORSOURCES_H
#define PULSATIONS_SENSORSOURCES_H

#include "ofMain.h"
#include "ofxOscMessage.h"
#include "Statistics.h"

struct sensor_settings_t {
    bool active;
    float accelerationThreshold;
};

struct sensor_frame_t {
    uint64_t time;
    vector<float> data;
    ofBuffer calibration;
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
    vector<ofxOscMessage> eventQueue;
};

class SensorSources {
public:
    void addIMU(string sid = "000", float accThreshold = 0.f);
    sensor_source_t getIMU(uint8_t index);
    bool parseOscFrame(ofxOscMessage &msg);

    vector<sensor_source_t> imu;
};


#endif //PULSATIONS_SENSORSOURCES_H
