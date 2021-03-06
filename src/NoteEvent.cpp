//
// Created by anton on 04/06/17.
//

#include "NoteEvent.h"

NoteEvent::NoteEvent(uint64_t time, uint64_t duration, float pitch, float velocity, uint8_t channel = 1, string triggerName = "", float triggerValue = .0f) {
    _isOff = false;

    _time = time;
    _duration = duration;
    _pitch = pitch;
    _velocity = velocity;
    _channel = channel;
    _triggerName = triggerName;
    _triggerValue = triggerValue;
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

string NoteEvent::getTriggerName() {
    return _triggerName;
}

float NoteEvent::getTriggerValue() {
    return _triggerValue;
}

uint8_t NoteEvent::getMidiPitch() {
    uint8_t pitch = (uint8_t)roundf(fabs(_pitch) * 127.f);
    return (uint8_t)(pitch > 127 ? 127 : pitch);
}

float NoteEvent::getVelocity() {
    return _velocity;
}

uint8_t NoteEvent::getMidiVelocity() {
    uint8_t velocity = (uint8_t)roundf(fabs(_velocity) * 127.f);
    return (uint8_t)(velocity > 127 ? 127 : velocity);
}

bool NoteEvent::isNoteOff() {
    return _isOff;
}

void NoteEvent::setNoteOff(bool off) {
    _isOff = off;
}