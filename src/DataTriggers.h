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

    Trigger3D* addTrigger(string name, string target, string targetSensorId, float threshold, bool absolute);
    Trigger3D* addTrigger(string name, string target, string targetSensorId, float lowThreshold, float highThreshold, bool absolute);
    Trigger3D* addTrigger(string name, string target, string targetSensorId, ofVec3f threshold, bool absolute);
    Trigger3D* addTrigger(string name, string target, string targetSensorId, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute);

    void draw();
    void addFrame(sensor_frame_t inputFrame);
    int getTriggerIndexByName(string name);

    sensor_trigger_3d_result_t getTriggerResult(uint8_t index);
    vector<sensor_trigger_3d_result_t> getAllTriggerResults();

    void threadedFunction();

private:
    vector<Trigger3D*> _triggers;
    vector<sensor_frame_t> _frames;
};


#endif //PULSATIONS_DATATRIGGERS_H
