//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_SENSOR_H
#define PULSATIONS_SENSOR_H

#include "ofMain.h"
#include "TimeKeeper.h"
#include "SensorGraph.h"
#include "Trigger3D.h"

class Sensor {
public:
    Sensor(string id, string name, string type, uint8_t index);

    void addFrame(sensor_frame_t frame);
    void addFrame(uint64_t time, uint64_t time_received, ofVec3f acceleration, ofVec3f orientation, ofVec4f quaternion);

    Trigger3D* addTrigger(string name, string target, float threshold, bool absolute);
    Trigger3D* addTrigger(string name, string target, float lowThreshold, float highThreshold, bool absolute);
    Trigger3D* addTrigger(string name, string target, ofVec3f threshold, bool absolute);
    Trigger3D* addTrigger(string name, string target, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute);

    void setCalibrationStatus(ofBuffer calibration);
    void setGraph(ofPoint position, float width, float height);
    void setBufferSizeMillis(uint64_t size);

    void update();
    void draw();
    bool hasOSCAddress(string address);
    bool hasFrames();

    string getSensorID();
    string getOSCAddress();
    string getDataAsString();
    string getCalibrationStatus();
    string getSystemStatus();

    vector<sensor_frame_t> getFrameBuffer();
    sensor_frame_t getCurrentFrame();
    vector<Trigger3D *> getTriggers();
    sensor_status_t getStatus();

private:
    string _id;
    string _type;
    string _name;
    uint8_t _index;

    sensor_status_t _status;
    bool _hasGraph;

    uint64_t _bufferTimeMillis;

    vector<sensor_frame_t> _frames;
    vector<Trigger3D *> _triggers;
    SensorGraph *_graph;
    TimeKeeper time;
};

#endif //PULSATIONS_SENSOR_H
