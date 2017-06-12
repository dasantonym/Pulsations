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
    _bufferSize = 100;
    _packetCount = 0;
    _lastPacketCount = 0;
    _pps = 0;
    _lastPPSTime = time.getTimeMillis();

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

    if (time.getTimeMillis() - _lastPPSTime >= 1000) {
        _lastPPSTime = time.getTimeMillis();
        _pps = _packetCount - _lastPacketCount;
        _lastPacketCount = _packetCount;
    }

    if (_frames.size() > _bufferSize) {
        _frames.erase(_frames.begin(), _frames.begin() + _frames.size() - _bufferSize);
    }

    if (_hasGraph) {
        _graph->update(getFrameBuffer());
    }
}

void Sensor::draw() {
    if (_hasGraph) {
        _graph->draw();
    }

    ofPushStyle();
    ofSetColor(140, 140, 140);
    ofDrawBitmapString(getDataAsString(), 40.f, 40.f + 160.f * _index);
    for (Trigger3D * trigger : _triggers) {
        ofDrawBitmapString(trigger->getTriggerAsString(),
                40.f, 40.f + 160.f * _index + 20.f * (trigger->getMeta()->trigger_id + 1));
    }
    ofPopStyle();

    if (hasFrames()) {
        ofPoint x1 = ofPoint(-20.f, 25.f);
        ofPoint x2 = ofPoint(20.f, 25.f);
        ofPoint y1 = ofPoint(.0f, -25.f);

        ofPushStyle();
        ofFill();
        ofSetColor(180, 0, 0);

        ofPushMatrix();
        ofTranslate(40.f + 35.f, 40.f + 160.f * (_index + 1) - 70.f);
        ofRotate(getCurrentFrame().orientation.x);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofSetColor(180, 180, 0);
        ofPushMatrix();
        ofTranslate(40.f + 35.f + 70.f, 40.f + 160.f * (_index + 1) - 70.f);
        ofRotate(getCurrentFrame().orientation.y);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofSetColor(0, 180, 180);
        ofPushMatrix();
        ofTranslate(40.f + 35.f + 70.f * 2.f, 40.f + 160.f * (_index + 1) - 70.f);
        ofRotate((getCurrentFrame().orientation.z * -1.f) + 90.f);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofPopStyle();
    }
}

void Sensor::addFrame(sensor_frame_t frame) {
    _frames.push_back(frame);
    _packetCount++;

    for (Trigger3D * trigger : _triggers) {
        if (trigger->getMeta()->target_type == "orientation") {
            trigger->update(frame.orientation);
        } else if (trigger->getMeta()->target_type == "acceleration") {
            trigger->update(frame.acceleration);
        } else if (trigger->getMeta()->target_type == "quaternion") {
            trigger->update(frame.quaternion);
        }
    }
}

void Sensor::addFrame(uint64_t time, uint64_t time_received, ofVec3f acceleration, ofVec3f orientation, ofVec4f quaternion) {
    sensor_frame_t frame;
    frame.time_received = time_received;
    frame.time = time;
    frame.acceleration = acceleration;
    frame.orientation = orientation;
    frame.quaternion = quaternion;
    frame.sensor_id = _id;
    addFrame(frame);
}



//
//
// Adding triggers
//

Trigger3D* Sensor::addTrigger(string name, string target_type, float threshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(threshold);
    trigger->getMeta()->name = name;
    trigger->getMeta()->target_type = target_type;
    trigger->getMeta()->sensor_id = _id;
    trigger->getMeta()->trigger_id = (uint16_t)_triggers.size();
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* Sensor::addTrigger(string name, string target_type, float lowThreshold, float highThreshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger->getMeta()->name = name;
    trigger->getMeta()->target_type = target_type;
    trigger->getMeta()->sensor_id = _id;
    trigger->getMeta()->trigger_id = (uint16_t)_triggers.size();
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* Sensor::addTrigger(string name, string target_type, ofVec3f threshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(threshold);
    trigger->getMeta()->name = name;
    trigger->getMeta()->target_type = target_type;
    trigger->getMeta()->sensor_id = _id;
    trigger->getMeta()->trigger_id = (uint16_t)_triggers.size();
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* Sensor::addTrigger(string name, string target_type, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger->getMeta()->name = name;
    trigger->getMeta()->target_type = target_type;
    trigger->getMeta()->sensor_id = _id;
    trigger->getMeta()->trigger_id = (uint16_t)_triggers.size();
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
    result += getSystemStatus() + "   ";
    if (hasFrames()) {
#ifdef RECEIVE_QUATERNION
        result += "QUAT:    ";
        result += ofToString(getCurrentFrame().quaternion.w, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().quaternion.x, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().quaternion.y, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().quaternion.z, 2, 7, ' ') + "   ";
#endif
        result += "EUL: ";
        result += ofToString(getCurrentFrame().orientation.x, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().orientation.y, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().orientation.z, 2, 7, ' ') + "   ";
        result += "ACC: ";
        result += ofToString(getCurrentFrame().acceleration.x, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().acceleration.y, 2, 7, ' ') + " ";
        result += ofToString(getCurrentFrame().acceleration.z, 2, 7, ' ') + "   ";
        result += ofToString(_packetCount, 2, 7, ' ') + " ";
        result += ofToString(_pps, 2, 7, ' ') + " ";
    }
    return result;
}

string Sensor::getCalibrationStatus() {
    char buf[255];
    sprintf(buf, "%u%u%u%u", _status.calibration.getData()[0], _status.calibration.getData()[1],
            _status.calibration.getData()[2], _status.calibration.getData()[3]);
    return "" + ofToString(buf);
}

string Sensor::getSystemStatus() {
    char buf[255];
    sprintf(buf, "%u%u%u", _status.system.getData()[0], _status.system.getData()[1], _status.system.getData()[2]);
    return "" + ofToString(buf);
}

vector<sensor_frame_t> * Sensor::getFrameBuffer() {
    return &_frames;
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