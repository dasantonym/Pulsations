//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_NOTEEVENT_H
#define PULSATIONS_NOTEEVENT_H

#include "ofxMidi.h"

class NoteEvent {
public:
    NoteEvent(uint64_t time, uint64_t duration, float pitch, float velocity);

    uint64_t getTime();
    uint64_t getEndTime();
    uint64_t getDuration();
    float getPitch();
    uint8_t getMidiPitch();
    float getVelocity();
    uint8_t getMidiVelocity();

private:
    uint64_t _time;
    uint64_t _duration;
    float _pitch;
    float _velocity;
};


#endif //PULSATIONS_NOTEEVENT_H
