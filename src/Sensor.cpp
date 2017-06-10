//
// Created by anton on 04/06/17.
//

#include "Sensor.h"


//
//
// Constructors
//

Sensor::Sensor(string id, string name, string type, uint8_t index) {
    _id = id;
    _name = name;
    _type = type;
    _index = index;

    _bufferTimeMillis = 2000;

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

    if (_hasGraph) {
        _graph->update(getFrameBuffer());
    }
}

void Sensor::draw() {
    if (_hasGraph) {
        _graph->draw();
    }

    if (hasFrames()) {
        ofDrawBitmapString(getDataAsString(), 40.f, 40.f + 90.f * _index);
        for (Trigger3D * trigger : _triggers) {
            ofDrawBitmapString(trigger->getTriggerAsString(),
                    40.f, 40.f + 90.f * _index + 20.f * (trigger->trigger->trigger_index + 1));
        }

        ofPoint x1 = ofPoint(-20.f, 25.f);
        ofPoint x2 = ofPoint(20.f, 25.f);
        ofPoint y1 = ofPoint(.0f, -25.f);

        ofPushStyle();
        ofFill();
        ofSetColor(180, 0, 0);

        ofPushMatrix();
        ofTranslate(40.f + 35.f, 40.f + 100.f * (_index+1) - 35.f);
        ofRotate(getCurrentFrame().orientation.x);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofSetColor(180, 180, 0);
        ofPushMatrix();
        ofTranslate(40.f + 35.f + 70.f, 40.f + 100.f * (_index+1) - 35.f);
        ofRotate(getCurrentFrame().orientation.y);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofSetColor(0, 180, 180);
        ofPushMatrix();
        ofTranslate(40.f + 35.f + 70.f * 2.f, 40.f + 100.f * (_index+1) - 35.f);
        ofRotate((getCurrentFrame().orientation.z * -1.f) + 90.f);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofPopStyle();
    }
}

void Sensor::addFrame(uint64_t time, uint64_t time_received, ofVec3f acceleration, ofVec3f orientation) {
    sensor_frame_t frame;
    frame.time_received = time_received;
    frame.time = time;
    frame.acceleration = acceleration;
    frame.orientation = orientation;
    frame.sensor_id = _id;
    _frames.push_back(frame);

    for (Trigger3D * trigger : _triggers) {
        if (trigger->trigger->target == "orientation") {
            trigger->update(orientation);
        }
        if (trigger->trigger->target == "acceleration") {
            trigger->update(acceleration);
        }
    }

    uint32_t count = 0;
    while (count < _frames.size() && time_received - _frames[count].time_received > _bufferTimeMillis) {
        count++;
    }

    if (count) {
        _frames.erase(_frames.begin(), _frames.begin() + count - 1);
    }
}



//
//
// Adding triggers
//

Trigger3D* Sensor::addTrigger(string name, string target, float threshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(threshold);
    trigger->trigger->name = name;
    trigger->trigger->target = target;
    trigger->trigger->target_sid = _id;
    trigger->trigger->trigger_index = (int)_triggers.size();
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* Sensor::addTrigger(string name, string target, float lowThreshold, float highThreshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger->trigger->name = name;
    trigger->trigger->target = target;
    trigger->trigger->target_sid = _id;
    trigger->trigger->trigger_index = (int)_triggers.size();
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* Sensor::addTrigger(string name, string target, ofVec3f threshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(threshold);
    trigger->trigger->name = name;
    trigger->trigger->target = target;
    trigger->trigger->target_sid = _id;
    trigger->trigger->trigger_index = (int)_triggers.size();
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* Sensor::addTrigger(string name, string target, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger->trigger->name = name;
    trigger->trigger->target = target;
    trigger->trigger->target_sid = _id;
    trigger->trigger->trigger_index = (int)_triggers.size();
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
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
    _status.calibration.getData()[0] = calibration.getData()[0];
    _status.calibration.getData()[1] = calibration.getData()[1];
    _status.calibration.getData()[2] = calibration.getData()[2];
    _status.calibration.getData()[3] = calibration.getData()[3];
}

void Sensor::setBufferSizeMillis(uint64_t size) {
    _bufferTimeMillis = size;
}


//
//
// Getters
//

bool Sensor::hasOSCAddress(string address) {
    vector<string> parts = ofSplitString(address, "/", true, true);
    return _type == parts[0] && _id == parts[1];
}

string Sensor::getSensorID() {
    return _id;
}

string Sensor::getOSCAddress() {
    return "/" + _type + "/" + _id;
}

string Sensor::getDataAsString() {
    string result = "";
    result += getOSCAddress() + "   ";
    result += getCalibrationStatus() + "   ";
    if (hasFrames()) {
        result += "Acc.: ";
        result += ofToString(getCurrentFrame().acceleration.x, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().acceleration.y, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().acceleration.z, 2, 7, ' ') + "   ";
        result += "Ort.: ";
        result += ofToString(getCurrentFrame().orientation.x, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().orientation.y, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().orientation.z, 2, 7, ' ');
    }
    return result;
}

string Sensor::getCalibrationStatus() {
    string result = "";
    uint8_t i = 0;
    for (char &c : _status.calibration) {
        if (i > 3) {
            continue;
        }
        if (c == 0x01) {
            result += "1";
        } else if (c == 0x02) {
            result += "2";
        } else if (c == 0x03) {
            result += "3";
        } else {
            result += "0";
        }
        i++;
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

sensor_status_t Sensor::getStatus() {
    return _status;
}

vector<Trigger3D *> Sensor::getTriggers() {
    return _triggers;
}