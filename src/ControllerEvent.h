//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_CONTROLLEREVENT_H
#define PULSATIONS_CONTROLLEREVENT_H

#include "ofxMidi.h"


class ControllerEvent {
public:
    ControllerEvent(uint64_t time, uint8_t controller, float value);

    uint64_t getTime();
    uint8_t getMidiValue();
    uint8_t getController();

private:
    uint64_t _time;
    uint8_t _controller;
    float _value;
};


#endif //PULSATIONS_CONTROLLEREVENT_H
