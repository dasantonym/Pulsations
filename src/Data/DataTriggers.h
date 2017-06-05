//
// Created by anton on 05/06/17.
//

#ifndef PULSATIONS_DATATRIGGERS_H
#define PULSATIONS_DATATRIGGERS_H

#include "ofMain.h"
#include "Trigger3D.h"
#include "Types.h"

class DataTriggers : public ofThread {
public:
    DataTriggers();

    sensor_trigger_3d_t addTrigger(string name, string target, string targetSensorId, float threshold, bool absolute);
    sensor_trigger_3d_t addTrigger(string name, string target, string targetSensorId, float lowThreshold, float highThreshold, bool absolute);
    sensor_trigger_3d_t addTrigger(string name, string target, string targetSensorId, ofVec3f threshold, bool absolute);
    sensor_trigger_3d_t addTrigger(string name, string target, string targetSensorId, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute);

    void addFrame(sensor_frame_t inputFrame);
    int getTriggerIndexByName(string name);
    vector<sensor_trigger_3d_result_t> getAllTriggerResults();

    void threadedFunction();

private:
    vector<sensor_trigger_3d_t> _triggers;
    vector<sensor_frame_t> _frames;
};


#endif //PULSATIONS_DATATRIGGERS_H
