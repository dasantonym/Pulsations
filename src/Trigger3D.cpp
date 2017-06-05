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
    setFalloff(.0f);
}

Trigger3D::Trigger3D(ofVec3f threshold) {
    setTrigger(threshold);
    setFalloff(.0f);
}

Trigger3D::Trigger3D(float threshold_low, float threshold_high) {
    setTrigger(threshold_low, threshold_high);
    setFalloff(.0f);
}

Trigger3D::Trigger3D(ofVec3f threshold_low, ofVec3f threshold_high) {
    setTrigger(threshold_low, threshold_high);
    setFalloff(.0f);
}

//
//
// Main
//

void Trigger3D::update(ofVec3f value) {
    ofVec3f newVal;
    uint64_t t = time.getTimeMillis();

    if (t - _lastTriggerTime.x > _trigger_debounce.x) {
        newVal.x = getTriggerValue(value.x, _threshold_low.x, _threshold_high.x, _trigger_mask.x);
        if (newVal.x != .0f) {
            _triggerValue.x = newVal.x;
            _lastTriggerTime.x = t;
        }
    } else if (_triggerValue.x != .0f) {
        bool isPos = _triggerValue.x > .0f;
        _triggerValue.x -= _triggerFalloff.x == .0f ? _triggerValue.x : _triggerFalloff.x;
        if (isPos && _triggerValue.x <= .0f) {
            _triggerValue.x = .0f;
        } else if (!isPos && _triggerValue.x >= .0f) {
            _triggerValue.x = .0f;
        }
    }
    if (t - _lastTriggerTime.y > _trigger_debounce.y) {
        newVal.y = getTriggerValue(value.y, _threshold_low.y, _threshold_high.y, _trigger_mask.y);
        if (newVal.y != .0f) {
            _triggerValue.y = newVal.y;
            _lastTriggerTime.y = t;
        }
    } else if (_triggerValue.y != .0f) {
        bool isPos = _triggerValue.y > 0;
        _triggerValue.y -= _triggerFalloff.y == .0f ? _triggerValue.y : _triggerFalloff.y;
        if (isPos && _triggerValue.y <= .0f) {
            _triggerValue.y = .0f;
        } else if (!isPos && _triggerValue.y >= .0f) {
            _triggerValue.y = .0f;
        }
    }
    if (t - _lastTriggerTime.z > _trigger_debounce.z) {
        newVal.z = getTriggerValue(value.z, _threshold_low.z, _threshold_high.z, _trigger_mask.z);
        if (newVal.z != .0f) {
            _triggerValue.z = newVal.z;
            _lastTriggerTime.z = t;
        }
    } else if (_triggerValue.z != .0f) {
        bool isPos = _triggerValue.z > 0;
        _triggerValue.z -= _triggerFalloff.z == .0f ? _triggerValue.z : _triggerFalloff.z;
        if (isPos && _triggerValue.z <= .0f) {
            _triggerValue.z = .0f;
        } else if (!isPos && _triggerValue.z >= .0f) {
             _triggerValue.z = .0f;
        }
    }
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

void Trigger3D::setFalloff(float falloff) {
    _triggerFalloff.x = falloff;
    _triggerFalloff.y = falloff;
    _triggerFalloff.z = falloff;
}

void Trigger3D::setFalloff(ofVec3f falloff) {
    _triggerFalloff = falloff;
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

void Trigger3D::setSensorInfo(uint8_t index) {
    _sensor_index = index;
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

uint8_t Trigger3D::getSensorIndex() {
    return _sensor_index;
}

ofVec3f Trigger3D::getDebounceStatus() {
    ofVec3f status = { .0f, .0f, .0f };
    uint64_t t = time.getTimeMillis();
    if (t - _lastTriggerTime.x > _trigger_debounce.x) {
        status.x = 1.f;
    }
    if (t - _lastTriggerTime.y > _trigger_debounce.y) {
        status.y = 1.f;
    }
    if (t - _lastTriggerTime.z > _trigger_debounce.z) {
        status.z = 1.f;
    }
    return status;
}

string Trigger3D::getTriggerAsString() {
    string result = "";
    result += ofToString(_triggerValue.x, 2, 7, ' ') + " ";
    result += ofToString(_triggerValue.y, 2, 7, ' ') + " ";
    result += ofToString(_triggerValue.z, 2, 7, ' ');
    return result;
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