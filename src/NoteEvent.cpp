//
// Created by anton on 04/06/17.
//

#include "NoteEvent.h"

NoteEvent::NoteEvent(uint64_t time, uint64_t duration, float pitch, float velocity, uint8_t channel = 1) {
    _time = time;
    _duration = duration;
    _pitch = pitch;
    _velocity = velocity;
    _channel = channel;
}

uint64_t NoteEvent::getTime() {
    return _time;
}

uint64_t NoteEvent::getEndTime() {
    return _time + _duration;
}

uint64_t NoteEvent::getDuration() {
    return _duration;
}

float NoteEvent::getPitch() {
    return _pitch;
}

uint8_t NoteEvent::getChannel() {
    return _channel;
}

uint8_t NoteEvent::getMidiPitch() {
    uint32_t pitch = roundf(fabs(_pitch) * 127.f);
    return (uint8_t)(pitch > 127 ? 127 : pitch);
}

float NoteEvent::getVelocity() {
    return _velocity;
}

uint8_t NoteEvent::getMidiVelocity() {
    uint32_t velocity = roundf(fabs(_velocity) * 127.f);
    return (uint8_t)(velocity > 127 ? 127 : velocity);
}
