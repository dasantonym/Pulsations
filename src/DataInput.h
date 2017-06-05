//
// Created by anton on 05/06/17.
//

#ifndef PULSATIONS_DATAINPUT_H
#define PULSATIONS_DATAINPUT_H

#include "ofMain.h"
#include "ofxOscReceiver.h"
#include "Sensor.h"

class DataInput : public ofThread {
public:
    DataInput(uint32_t udpPort);

    Sensor* addBNO055Source(string id, string name, string type, uint8_t index);
    uint64_t sourceCount();
    bool sourceHasFrames(uint16_t index);
    sensor_frame_t getNextFrameFromSource(uint16_t index);

    void draw();

    void threadedFunction();

private:
    ofxOscReceiver _oscInput;
    vector<Sensor*> _sources;

    TimeKeeper time;
};


#endif //PULSATIONS_DATAINPUT_H
