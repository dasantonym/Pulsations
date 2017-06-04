//
// Created by anton on 04/06/17.
//

#include "Trigger3D.h"

//
//
// Constructors
//

Trigger3D::Trigger3D(float threshold) {
    setTrigger(threshold);
}

Trigger3D::Trigger3D(ofVec3f threshold) {
    setTrigger(threshold);
}

Trigger3D::Trigger3D(float threshold_low, float threshold_high) {
    setTrigger(threshold_low, threshold_high);
}

Trigger3D::Trigger3D(ofVec3f threshold_low, ofVec3f threshold_high) {
    _isRange = true;
    _threshold_low = threshold_low;
    _threshold_high = threshold_high;
}

//
//
// Main
//

void Trigger3D::update(ofVec3f value) {
    _triggerValue.x = getTriggerValue(value.x, _threshold_low.x, _threshold_high.x, _trigger_mask.x);
    _triggerValue.y = getTriggerValue(value.y, _threshold_low.y, _threshold_high.y, _trigger_mask.y);
    _triggerValue.z = getTriggerValue(value.z, _threshold_low.z, _threshold_high.z, _trigger_mask.z);
}


//
//
// Setters
//

void Trigger3D::setTrigger(float threshold) {
    _threshold_low.x = threshold;
    _threshold_low.y = threshold;
    _threshold_low.z = threshold;
}

void Trigger3D::setTrigger(ofVec3f threshold) {
    _threshold_low = threshold;
}

void Trigger3D::setTrigger(float lowThreshold, float highThreshold) {
    _isRange = true;
    _threshold_low.x = lowThreshold;
    _threshold_low.y = lowThreshold;
    _threshold_low.z = lowThreshold;
    _threshold_high.x = highThreshold;
    _threshold_high.y = highThreshold;
    _threshold_high.z = highThreshold;
}

void Trigger3D::setTrigger(ofVec3f lowThreshold, ofVec3f highThreshold) {
    _isRange = true;
    _threshold_low = lowThreshold;
    _threshold_high = highThreshold;
}

void Trigger3D::setDebounce(float millis) {
    _trigger_debounce.x = millis;
    _trigger_debounce.y = millis;
    _trigger_debounce.z = millis;
}

void Trigger3D::setDebounce(ofVec3f millis) {
    _trigger_debounce = millis;
}

void Trigger3D::setMask(float mask) {
    _trigger_mask.x = mask;
    _trigger_mask.y = mask;
    _trigger_mask.z = mask;
}

void Trigger3D::setMask(ofVec3f mask) {
    _trigger_mask = mask;
}

void Trigger3D::setAbsolute(bool isAbsolute) {
    _isAbsolute = isAbsolute;
}

//
//
// Getters
//

bool Trigger3D::isTriggered() {
    if (_triggerValue.x > .0f) {
        return true;
    }
    if (_triggerValue.y > .0f) {
        return true;
    }
    if (_triggerValue.z > .0f) {
        return true;
    }
}

ofVec3f Trigger3D::getTrigger() {
    return _triggerValue;
}

float Trigger3D::getTriggerMax() {
    float result = .0f;
    result = _triggerValue.x > result ? _triggerValue.x : result;
    result = _triggerValue.y > result ? _triggerValue.y : result;
    result = _triggerValue.z > result ? _triggerValue.z : result;
    return result;
}

float Trigger3D::getTriggerMin() {
    float result = .0f;
    result = _triggerValue.x < result ? _triggerValue.x : result;
    result = _triggerValue.y < result ? _triggerValue.y : result;
    result = _triggerValue.z < result ? _triggerValue.z : result;
    return result;
}

float Trigger3D::getTriggerValue(float value, float threshold_low, float threshold_high = .0f, float mask = .0f) {
    float result = .0f;
    if (_isAbsolute) {
        value = fabs(value);
    }
    if (mask == .0f && value >= threshold_low) {
        bool add = true;
        if (_isRange) {
            if (value >= threshold_high) {
                add = false;
            }
        }
        if (add) {
            if (_isRange) {
                result = value - threshold_low / (threshold_high - threshold_low);
            } else {
                result = value - threshold_low;
            }
        }
    }
    return result;
}