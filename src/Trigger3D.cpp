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
    trigger = new sensor_trigger_3d_t();
}

Trigger3D::Trigger3D(ofVec3f threshold) {
    setTrigger(threshold);
    setFalloff(.0f);
    trigger = new sensor_trigger_3d_t();
}

Trigger3D::Trigger3D(float threshold_low, float threshold_high) {
    setTrigger(threshold_low, threshold_high);
    setFalloff(.0f);
    trigger = new sensor_trigger_3d_t();
}

Trigger3D::Trigger3D(ofVec3f threshold_low, ofVec3f threshold_high) {
    setTrigger(threshold_low, threshold_high);
    setFalloff(.0f);
    trigger = new sensor_trigger_3d_t();
}

//
//
// Main
//

void Trigger3D::update(ofVec3f value) {
    uint64_t t = time.getTimeMillis();

    ofVec3f triggerValue;
    triggerValue.x = t - _lastTriggerTime.x > _trigger_debounce.x ?
            getTriggerValue(value.x, _threshold_low.x, _threshold_high.x, _trigger_mask.x) : .0f;
    triggerValue.y = t - _lastTriggerTime.y > _trigger_debounce.y ?
            getTriggerValue(value.y, _threshold_low.y, _threshold_high.y, _trigger_mask.y) : .0f;
    triggerValue.z = t - _lastTriggerTime.z > _trigger_debounce.z ?
            getTriggerValue(value.z, _threshold_low.z, _threshold_high.z, _trigger_mask.z) : .0f;

    _triggerValueLastMax.x = triggerValue.x > _triggerValueLastMax.x ? triggerValue.x : _triggerValueLastMax.x;
    _triggerValueLastMax.y = triggerValue.y > _triggerValueLastMax.y ? triggerValue.y : _triggerValueLastMax.y;
    _triggerValueLastMax.z = triggerValue.z > _triggerValueLastMax.z ? triggerValue.z : _triggerValueLastMax.z;

    if (time.getTimeMillis() - _lastUpdate >= 40) {
        _lastUpdate = time.getTimeMillis();
        bool send = false;

        if (t - _lastTriggerTime.x > _trigger_debounce.x && _triggerValue.x != .0f && _triggerValueLastMax.x > _triggerValue.x) {
            _triggerValue.x = _triggerValue.x < _triggerValueLastMax.x ? triggerValue.x : _triggerValueLastMax.x;
            _lastTriggerTime.x = _lastUpdate;
            send = true;
        }
        if (t - _lastTriggerTime.y > _trigger_debounce.y && _triggerValue.y != .0f) {
            _triggerValue.y = _triggerValue.y < _triggerValueLastMax.y ? triggerValue.y : _triggerValueLastMax.y;
            _lastTriggerTime.y = _lastUpdate;
            send = true;
        }
        if (t - _lastTriggerTime.z > _trigger_debounce.z && _triggerValue.z != .0f) {
            _triggerValue.z = _triggerValue.z < _triggerValueLastMax.z ? triggerValue.z : _triggerValueLastMax.z;
            _lastTriggerTime.z = _lastUpdate;
            send = true;
        }

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

        _triggerValueLastMax = { .0f, .0f, .0f };
    } else {
        _triggerValue.x *= _triggerFalloff.x == .0f ? 1.f : _triggerFalloff.x;
        _triggerValue.y *= _triggerFalloff.y == .0f ? 1.f : _triggerFalloff.y;
        _triggerValue.z *= _triggerFalloff.z == .0f ? 1.f : _triggerFalloff.z;
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

ofVec3f Trigger3D::getTrigger() {
    return _triggerValue;
}

uint8_t Trigger3D::getSensorIndex() {
    return _sensor_index;
}

ofVec3f Trigger3D::getDebounceStatus() {
    ofVec3f status = { .0f, .0f, .0f };
    uint64_t t = time.getTimeMillis();
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
                result = value;
            }
        }
    }
    return result;
}
