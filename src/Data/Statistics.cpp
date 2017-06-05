//
// Created by anton on 02/06/17.
//

#include "Statistics.h"

void Statistics::update(sensor_stats_t *stats) {
    while (stats->accelerationValues.size() > 0 && ofGetElapsedTimeMillis() - stats->accelerationValues[0].w > stats->bufferTimeMillis) {
        stats->accelerationValues.erase(stats->accelerationValues.begin(), stats->accelerationValues.begin() + 1);
    }
    while (stats->orientationValues.size() > 0 && ofGetElapsedTimeMillis() - stats->orientationValues[0].w > stats->bufferTimeMillis) {
        stats->orientationValues.erase(stats->orientationValues.begin(), stats->orientationValues.begin() + 1);
    }
    for (ofVec4f & vec : stats->accelerationValues) {
        stats->accelerationAvg.x += vec.x;
        stats->accelerationAvg.y += vec.y;
        stats->accelerationAvg.z += vec.z;
        if (vec.x > stats->accelerationMax.x) {
            stats->accelerationMax.x = vec.x;
        }
        if (vec.y > stats->accelerationMax.y) {
            stats->accelerationMax.y = vec.y;
        }
        if (vec.z > stats->accelerationMax.z) {
            stats->accelerationMax.z = vec.z;
        }
        if (vec.x < stats->accelerationMin.x) {
            stats->accelerationMin.x = vec.x;
        }
        if (vec.y < stats->accelerationMin.y) {
            stats->accelerationMin.y = vec.y;
        }
        if (vec.z < stats->accelerationMin.z) {
            stats->accelerationMin.z = vec.z;
        }
    }

    if (stats->accelerationMax.x > stats->accelerationMaxGlobal) {
        stats->accelerationMaxGlobal = stats->accelerationMax.x;
    }
    if (stats->accelerationMax.y > stats->accelerationMaxGlobal) {
        stats->accelerationMaxGlobal = stats->accelerationMax.y;
    }
    if (stats->accelerationMax.y > stats->accelerationMaxGlobal) {
        stats->accelerationMaxGlobal = stats->accelerationMax.y;
    }
    if (stats->accelerationMin.x < stats->accelerationMinGlobal) {
        stats->accelerationMinGlobal = stats->accelerationMin.x;
    }
    if (stats->accelerationMin.y > stats->accelerationMinGlobal) {
        stats->accelerationMinGlobal = stats->accelerationMin.y;
    }
    if (stats->accelerationMin.y > stats->accelerationMinGlobal) {
        stats->accelerationMinGlobal = stats->accelerationMin.y;
    }

    stats->accelerationAvg.x = stats->accelerationValues.size() ? stats->accelerationAvg.x / (float) stats->accelerationValues.size() : 0.f;
    stats->accelerationAvg.y = stats->accelerationValues.size() ? stats->accelerationAvg.y / (float) stats->accelerationValues.size() : 0.f;
    stats->accelerationAvg.z = stats->accelerationValues.size() ? stats->accelerationAvg.z / (float) stats->accelerationValues.size() : 0.f;

    stats->accelerationAvgGlobal = ( stats->accelerationAvg.x + stats->accelerationAvg.y + stats->accelerationAvg.z) / 3.f;
}