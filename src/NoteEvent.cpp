//
// Created by anton on 04/06/17.
//

#include "NoteEvent.h"

NoteEvent::NoteEvent(uint64_t time, uint64_t duration, float pitch, float velocity) {
    _time = time;
    _duration = duration;
    _pitch = pitch;
    _velocity = velocity;
}

uint64_t NoteEvent::getTime() {
    return _time;
}

uint64_t NoteEvent::getEndTime() {
    return _time + _duration;
}

uint8_t NoteEvent::getMidiPitch() {
    return (uint8_t)roundf(_pitch * 128.f);
}

uint8_t NoteEvent::getMidiVelocity() {
    return (uint8_t)roundf(_velocity * 128.f);
}