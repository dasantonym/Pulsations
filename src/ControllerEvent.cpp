//
// Created by anton on 04/06/17.
//

#include "ControllerEvent.h"

ControllerEvent::ControllerEvent(uint64_t time, uint8_t controller, float value) {
    _time = time;
    _controller = controller;
    _value = value > 1.f ? 1.f : value < .0f ? .0f : value;
}

uint64_t ControllerEvent::getTime() {
    return _time;
}

uint8_t ControllerEvent::getMidiValue() {
    return (uint8_t)roundf(_value * 128);
}

uint8_t ControllerEvent::getController() {
    return _controller;
}