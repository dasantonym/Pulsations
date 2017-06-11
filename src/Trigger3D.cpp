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
    trigger = new sensor_trigger_4d_t();
    _lastTriggerTime = { 0, 0, 0, 0 };
}

Trigger3D::Trigger3D(ofVec4f threshold) {
    setTrigger(threshold);
    setFalloff(.0f);
    trigger = new sensor_trigger_4d_t();
    _lastTriggerTime = { 0, 0, 0, 0 };
}

Trigger3D::Trigger3D(float threshold_low, float threshold_high) {
    setTrigger(threshold_low, threshold_high);
    setFalloff(.0f);
    trigger = new sensor_trigger_4d_t();
    _lastTriggerTime = { 0, 0, 0, 0 };
}

Trigger3D::Trigger3D(ofVec4f threshold_low, ofVec4f threshold_high) {
    setTrigger(threshold_low, threshold_high);
    setFalloff(.0f);
    trigger = new sensor_trigger_4d_t();
    _lastTriggerTime = { 0, 0, 0, 0 };
}

//
//
// Main
//

void Trigger3D::update(ofVec3f value) {
    ofVec4f vec;
    vec.x = value.x;
    vec.y = value.y;
    vec.z = value.z;
    vec.w = .0f;
    update(vec);
}

void Trigger3D::update(ofVec4f value) {
    ofVec4f triggerValue;
    triggerValue.x = getTriggerValue(value.x, _threshold_low.x, _threshold_high.x, _trigger_mask.x);
    triggerValue.y = getTriggerValue(value.y, _threshold_low.y, _threshold_high.y, _trigger_mask.y);
    triggerValue.z = getTriggerValue(value.z, _threshold_low.z, _threshold_high.z, _trigger_mask.z);
    triggerValue.w = getTriggerValue(value.w, _threshold_low.w, _threshold_high.w, _trigger_mask.w);

    if (time.getTimeMillis() - _lastUpdate >= 40) {
        _lastUpdate = time.getTimeMillis();
        //bool send = false;

        if (time.getTimeMillis() - _lastTriggerTime.x > _trigger_debounce.x) {
            _triggerValue.x = triggerValue.x;
            if (_triggerValue.x != .0f) {
                _lastTriggerTime.x = time.getTimeMillis();
            }
            //send = true;
        }

        if (time.getTimeMillis() - _lastTriggerTime.y > _trigger_debounce.y) {
            _triggerValue.y = triggerValue.y;
            if (_triggerValue.y != .0f) {
                _lastTriggerTime.y = time.getTimeMillis();
            }
            //send = true;
        }

        if (time.getTimeMillis() - _lastTriggerTime.z > _trigger_debounce.z) {
            _triggerValue.z = triggerValue.z;
            if (_triggerValue.z != .0f) {
                _lastTriggerTime.z = time.getTimeMillis();
            }
            //send = true;
        }

        /*
        if (send) {
            sensor_trigger_3d_result_t result;
            result.target_sid = trigger->target_sid;
            result.target = trigger->target;
            result.name = trigger->name;
            result.debounceStatus = getDebounceStatus();
            result.isTriggered = isTriggered();
            result.index = _sensor_index;
            result.triggerValue = _triggerValue;
            ofNotifyEvent(triggerValueEvent, result);
        }
         */
    } else {
        /*
        _triggerValue.x *= _triggerFalloff.x == .0f ? 1.f : _triggerFalloff.x;
        _triggerValue.y *= _triggerFalloff.y == .0f ? 1.f : _triggerFalloff.y;
        _triggerValue.z *= _triggerFalloff.z == .0f ? 1.f : _triggerFalloff.z;
         */
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

void Trigger3D::setTrigger(ofVec4f threshold) {
    _threshold_low = threshold;
}

void Trigger3D::setTrigger(float lowThreshold, float highThreshold) {
    _isRange = true;
    _threshold_low.w = lowThreshold;
    _threshold_low.x = lowThreshold;
    _threshold_low.y = lowThreshold;
    _threshold_low.z = lowThreshold;
    _threshold_high.w = highThreshold;
    _threshold_high.x = highThreshold;
    _threshold_high.y = highThreshold;
    _threshold_high.z = highThreshold;
}

void Trigger3D::setTrigger(ofVec4f lowThreshold, ofVec4f highThreshold) {
    _isRange = true;
    _threshold_low = lowThreshold;
    _threshold_high = highThreshold;
}

void Trigger3D::setFalloff(float falloff) {
    _triggerFalloff.w = falloff;
    _triggerFalloff.x = falloff;
    _triggerFalloff.y = falloff;
    _triggerFalloff.z = falloff;
}

void Trigger3D::setFalloff(ofVec4f falloff) {
    _triggerFalloff = falloff;
}

void Trigger3D::setDebounce(float millis) {
    _trigger_debounce.w = millis;
    _trigger_debounce.x = millis;
    _trigger_debounce.y = millis;
    _trigger_debounce.z = millis;
}

void Trigger3D::setDebounce(ofVec4f millis) {
    _trigger_debounce = millis;
}

void Trigger3D::setMask(float mask) {
    _trigger_mask.w = mask;
    _trigger_mask.x = mask;
    _trigger_mask.y = mask;
    _trigger_mask.z = mask;
}

void Trigger3D::setMask(ofVec4f mask) {
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
    if (_triggerValue.w != .0f) {
        return true;
    }
    if (_triggerValue.x != .0f) {
        return true;
    }
    if (_triggerValue.y != .0f) {
        return true;
    }
    if (_triggerValue.z != .0f) {
        return true;
    }
}

ofVec4f Trigger3D::getTrigger() {
    ofVec4f triggerValue;
    triggerValue.w = _triggerValue.w;
    triggerValue.x = _triggerValue.x;
    triggerValue.y = _triggerValue.y;
    triggerValue.z = _triggerValue.z;
    return triggerValue;
}

uint8_t Trigger3D::getSensorIndex() {
    return _sensor_index;
}

ofVec4f Trigger3D::getDebounceStatus() {
    ofVec4f status = { .0f, .0f, .0f, .0f };
    uint64_t t = time.getTimeMillis();
    if (t - _lastTriggerTime.w >= _trigger_debounce.w) {
        status.w = 1.f;
    }
    if (t - _lastTriggerTime.x >= _trigger_debounce.x) {
        status.x = 1.f;
    }
    if (t - _lastTriggerTime.y >= _trigger_debounce.y) {
        status.y = 1.f;
    }
    if (t - _lastTriggerTime.z >= _trigger_debounce.z) {
        status.z = 1.f;
    }
    return status;
}

string Trigger3D::getTriggerAsString() {
    string result = "                    TRIG #" + ofToString(trigger->trigger_index) + ": ";
    result += ofToString(_triggerValue.x, 2, 7, ' ') + " ";
    result += ofToString(_triggerValue.y, 2, 7, ' ') + " ";
    result += ofToString(_triggerValue.z, 2, 7, ' ') + " ";
    result += ofToString(_triggerValue.w, 2, 7, ' ');
    return result;
}

float Trigger3D::getTriggerMax() {
    float result = .0f;
    result = _triggerValue.x > result ? _triggerValue.x : result;
    result = _triggerValue.y > result ? _triggerValue.y : result;
    result = _triggerValue.z > result ? _triggerValue.z : result;
    result = _triggerValue.w > result ? _triggerValue.w : result;
    return result;
}

float Trigger3D::getTriggerMin() {
    float result = .0f;
    result = _triggerValue.x < result ? _triggerValue.x : result;
    result = _triggerValue.y < result ? _triggerValue.y : result;
    result = _triggerValue.z < result ? _triggerValue.z : result;
    result = _triggerValue.w < result ? _triggerValue.w : result;
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
            /*
            if (_isRange) {
                result = value;
            } else {
                result = value;
            }
             */
            result = value;
        }
    }
    return result;
}

sensor_trigger_4d_result_t Trigger3D::getTriggerResult() {
    sensor_trigger_4d_result_t res;
    res.triggerValue = getTrigger();
    res.target_sid = trigger->target_sid;
    res.trigger_index = trigger->trigger_index;
    res.target = trigger->target;
    res.name = trigger->name;
    res.isTriggered = isTriggered();
    res.debounceStatus = getDebounceStatus();
    return res;
}