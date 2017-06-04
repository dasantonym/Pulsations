//
// Created by anton on 04/06/17.
//

#include "Sensor.h"


//
//
// Constructors
//

Sensor::Sensor(string id, string name, string type) {
    _id = id;
    _name = name;
    _type = type;

    _bufferTimeMillis = 1000;

    _status.active = true;
    _status.calibration = { 0x00, 0x00, 0x00, 0x00 };

    _hasGraph = false;
}


//
//
// Main
//

void Sensor::update() {
    if (!hasFrames()) {
        return;
    }
    for (sensor_trigger_3d_t & trigger : _triggers) {
        if (trigger.target == "acceleration") {
            trigger.trigger->update(getCurrentFrame().acceleration);
        }
        if (trigger.target == "orientation") {
            trigger.trigger->update(getCurrentFrame().orientation);
        }
    }
    if (_hasGraph) {
        _graph->update(getFrameBuffer());
    }
}

void Sensor::draw() {
    if (_hasGraph) {
        _graph->draw();
    }
}

void Sensor::addFrame(uint64_t time, uint64_t time_received, ofVec3f acceleration, ofVec3f orientation) {
    _lastTime = time_received;
    sensor_frame_t frame;
    frame.time_received = time_received;
    frame.time = time;
    frame.acceleration = acceleration;
    frame.orientation = orientation;
    _frames.push_back(frame);
    uint32_t count = 0;
    while (time_received - _frames[count].time > _bufferTimeMillis) {
        count++;
    }
    if (count > 0) {
        _frames.erase(_frames.begin(), _frames.begin() + count);
    }
}


//
//
// Adding triggers
//

void Sensor::addTrigger(string name, string target, float threshold, bool absolute = false) {
    sensor_trigger_3d_t trigger;
    trigger.name = name;
    trigger.target = target;
    trigger.trigger = new Trigger3D(threshold);
    trigger.trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
}

void Sensor::addTrigger(string name, string target, float lowThreshold, float highThreshold, bool absolute = false) {
    sensor_trigger_3d_t trigger;
    trigger.name = name;
    trigger.target = target;
    trigger.trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger.trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
}

void Sensor::addTrigger(string name, string target, ofVec3f threshold, bool absolute = false) {
    sensor_trigger_3d_t trigger;
    trigger.name = name;
    trigger.target = target;
    trigger.trigger = new Trigger3D(threshold);
    trigger.trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
}

void Sensor::addTrigger(string name, string target, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute = false) {
    sensor_trigger_3d_t trigger;
    trigger.name = name;
    trigger.target = target;
    trigger.trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger.trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
}


//
//
// Setters
//

void Sensor::setGraph(ofPoint position, float width, float height) {
    _graph = new SensorGraph(position, width, height);
    _hasGraph = true;
}

void Sensor::setCalibrationStatus(ofBuffer calibration) {
    _status.calibration = calibration;
}

void Sensor::setActive(bool active) {
    _status.active = active;
}


//
//
// Getters
//

bool Sensor::hasOSCAddress(string address) {
    vector<string> parts = ofSplitString(address, "/", true, true);
    return _type == parts[0] && _id == parts[1];
}

string Sensor::getOSCAddress() {
    return "/" + _type + "/" + _id;
}

string Sensor::getCalibrationStatus() {
    string result = "";
    for (char &c : _status.calibration) {
        if (c == 0x1) {
            result += "1";
        } else if (c == 0x2) {
            result += "2";
        } else if (c == 0x3) {
            result += "3";
        } else {
            result += "0";
        }
    }
    return result;
}

vector<sensor_frame_t> Sensor::getFrameBuffer() {
    return _frames;
}

bool Sensor::hasFrames() {
    return _frames.size() > 0;
}

sensor_frame_t Sensor::getCurrentFrame() {
    return _frames[_frames.size() - 1];
}

vector<sensor_trigger_3d_t> Sensor::getTriggers() {
    return _triggers;
}

int Sensor::getTriggerIndexByName(string name) {
    int result = -1;
    int count = 0;
    for (sensor_trigger_3d_t & trigger : _triggers) {
        if (trigger.name == name) {
            result = count;
            return result;
        }
        count++;
    }
    return result;
}

sensor_status_t Sensor::getStatus() {
    return _status;
}