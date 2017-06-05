//
// Created by anton on 02/06/17.
//

#ifndef PULSATIONS_STATISTICS_H
#define PULSATIONS_STATISTICS_H

#include "ofMain.h"

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

class Statistics {
public:
    static void update(sensor_stats_t *stats);
};


#endif //PULSATIONS_STATISTICS_H
