//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_SENSOR_H
#define PULSATIONS_SENSOR_H

#include "ofMain.h"
#include "TimeKeeper.h"
#include "SensorGraph.h"

class Sensor {
public:
    Sensor(string id, string name, string type, uint8_t index);

    void parseOSCMessage(ofxOscMessage &msg);
    void addFrame(uint64_t time, uint64_t time_received, ofVec3f acceleration, ofVec3f orientation);
    /*
    void addTrigger(string name, string target, float threshold, bool absolute);
    void addTrigger(string name, string target, float lowThreshold, float highThreshold, bool absolute);
    void addTrigger(string name, string target, ofVec3f threshold, bool absolute);
    void addTrigger(string name, string target, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute);
     */
    void setCalibrationStatus(ofBuffer calibration);
    void setActive(bool active);
    void setGraph(ofPoint position, float width, float height);
    void setBufferSizeMillis(uint64_t size);

    void update();
    void draw();
    bool hasOSCAddress(string address);
    bool hasFrames();

    string getSensorID();
    string getOSCAddress();
    string getDataAsString();
    // string getTriggersAsString();
    string getCalibrationStatus();
    vector<sensor_frame_t> getFrameBuffer();
    sensor_frame_t getCurrentFrame();
    //vector<sensor_trigger_3d_t> getTriggers();
    //int getTriggerIndexByName(string name);
    sensor_status_t getStatus();

private:
    string _id;
    string _type;
    string _name;
    uint8_t _index;

    sensor_status_t _status;
    bool _active;
    bool _hasGraph;
    ofBuffer _calibration;

    uint64_t  _bufferTimeMillis;
    uint64_t _lastTime;

    vector<sensor_frame_t> _frames;
    // vector<sensor_trigger_3d_t> _triggers;
    SensorGraph *_graph;
    TimeKeeper time;
};

#endif //PULSATIONS_SENSOR_H
