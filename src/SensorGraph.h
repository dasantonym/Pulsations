//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_SENSORGRAPH_H
#define PULSATIONS_SENSORGRAPH_H

#include "ofMain.h"
#include "Types.h"

class SensorGraph {
public:
    SensorGraph(ofPoint position, float width, float height);

    void update(vector<sensor_frame_t> frames);
    void draw();

private:
    vector<ofPath> _paths;
    ofPoint _position;
    ofPoint _size;
};


#endif //PULSATIONS_SENSORGRAPH_H
