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
    _lastTriggerTime = { 0, 0, 0 };
}

Trigger3D::Trigger3D(ofVec3f threshold) {
    setTrigger(threshold);
    setFalloff(.0f);
    _lastTriggerTime = { 0, 0, 0 };
}

Trigger3D::Trigger3D(float threshold_low, float threshold_high) {
    setTrigger(threshold_low, threshold_high);
    setFalloff(.0f);
    _lastTriggerTime = { 0, 0, 0 };
}

Trigger3D::Trigger3D(ofVec3f threshold_low, ofVec3f threshold_high) {
    setTrigger(threshold_low, threshold_high);
    setFalloff(.0f);
    _lastTriggerTime = { 0, 0, 0 };
}

//
//
// Main
//

void Trigger3D::update(ofVec3f value) {
    if (time.getTimeMillis() - _lastUpdate >= 40) {
        _lastUpdate = time.getTimeMillis();

        if (time.getTimeMillis() - _lastTriggerTime.x > _trigger_debounce.x) {
            _triggerValue.x = getTriggerValue(value.x, _threshold_low.x, _threshold_high.x, _trigger_mask.x);
            if (_triggerValue.x != .0f) {
                _lastTriggerTime.x = time.getTimeMillis();
            }
        }

        if (time.getTimeMillis() - _lastTriggerTime.y > _trigger_debounce.y) {
            _triggerValue.y = getTriggerValue(value.y, _threshold_low.y, _threshold_high.y, _trigger_mask.y);
            if (_triggerValue.y != .0f) {
                _lastTriggerTime.y = time.getTimeMillis();
            }
        }

        if (time.getTimeMillis() - _lastTriggerTime.z > _trigger_debounce.z) {
            _triggerValue.z = getTriggerValue(value.z, _threshold_low.z, _threshold_high.z, _trigger_mask.z);
            if (_triggerValue.z != .0f) {
                _lastTriggerTime.z = time.getTimeMillis();
            }
        }

        update();
    }
}

void Trigger3D::update() {
    if (_triggerFalloff.x) _triggerValue.x *= _triggerFalloff.x;
    if (_triggerFalloff.y) _triggerValue.y *= _triggerFalloff.y;
    if (_triggerFalloff.z) _triggerValue.z *= _triggerFalloff.z;
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
    ofVec3f debounce = getDebounceStatus();
    if (_triggerValue.x && debounce.x) {
        return true;
    }
    if (_triggerValue.y != .0f && debounce.y == 1.f) {
        return true;
    }
    if (_triggerValue.z != .0f && debounce.z == 1.f) {
        return true;
    }
    return false;
}

sensor_trigger_t* Trigger3D::getMeta() {
    return &_meta;
}

ofVec3f Trigger3D::getTrigger() {
    ofVec4f triggerValue;
    triggerValue.x = _triggerValue.x;
    triggerValue.y = _triggerValue.y;
    triggerValue.z = _triggerValue.z;
    return triggerValue;
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
    string result = "TRIG #" + ofToString(_meta.trigger_id) + ": ";
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
    bool add;
    if (_isAbsolute) {
        add = mask == .0f && fabs(value) >= fabs(threshold_low) && (!_isRange || fabs(value) < fabs(threshold_high));
    } else {
        if (threshold_low < 0 || threshold_high < 0) {
            add = mask == .0f && value <= threshold_low && (!_isRange || value > threshold_high);
        } else {
            add = mask == .0f && value >= threshold_low && (!_isRange || value < threshold_high);
        }
    }
    if (add) {
        result = value;
    }
    return result;
}

sensor_trigger_result_3f_t Trigger3D::getTriggerResult() {
    sensor_trigger_result_3f_t res;
    res.triggerValue = getTrigger();
    res.trigger = _meta;
    res.isTriggered = isTriggered();
    res.debounceStatus = getDebounceStatus();
    return res;
}