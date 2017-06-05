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
    if (_hasGraph) {
        _graph->update(getFrameBuffer());
    }
}

void Sensor::draw() {
    if (_hasGraph) {
        _graph->draw();
    }

    if (hasFrames()) {
        ofDrawBitmapString(getDataAsString(), 40.f, 40.f + 90.f * (_index - 1));

        /*
        uint32_t tc = 0;
        for (sensor_trigger_3d_t * trigger : _triggers) {
            if (trigger->target_sid == getSensorID()) {
                ofDrawBitmapString(trigger->name + " " + trigger->trigger->getTriggerAsString() + "   ",
                        40.f + 160.f * tc, 40.f + 90.f * (count + 1) - 70.f);
                tc++;
            }
        }
         */

        ofPoint x1 = ofPoint(-20.f, 25.f);
        ofPoint x2 = ofPoint(20.f, 25.f);
        ofPoint y1 = ofPoint(.0f, -25.f);

        ofPushStyle();
        ofFill();
        ofSetColor(180, 0, 0);

        ofPushMatrix();
        ofTranslate(40.f + 35.f, 40.f + 100.f * _index - 40.f);
        ofRotate(getCurrentFrame().orientation.x);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofSetColor(180, 180, 0);
        ofPushMatrix();
        ofTranslate(40.f + 35.f + 70.f, 40.f + 100.f * _index - 40.f);
        ofRotate(getCurrentFrame().orientation.y);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofSetColor(0, 180, 180);
        ofPushMatrix();
        ofTranslate(40.f + 35.f + 70.f * 2.f, 40.f + 100.f * _index - 40.f);
        ofRotate((getCurrentFrame().orientation.z * -1.f) + 90.f);
        ofDrawTriangle(x1, x2, y1);
        ofPopMatrix();

        ofPopStyle();
    }
}

void Sensor::parseOSCMessage(ofxOscMessage &msg) {
    uint64_t timetag = time.getTimeMillis();
    if (hasOSCAddress(msg.getAddress())) {
        ofVec3f acceleration, orientation;
        for (int i = 1; i < msg.getNumArgs(); ++i) {
            string type = msg.getArgTypeName(i);
            if (type == "f") {
                switch (i) {
                    case 1:
                        orientation.x = msg.getArgAsFloat(i);
                        break;
                    case 2:
                        orientation.y = msg.getArgAsFloat(i);
                        break;
                    case 3:
                        orientation.z = msg.getArgAsFloat(i);
                        break;
                    case 4:
                        acceleration.x = msg.getArgAsFloat(i);
                        break;
                    case 5:
                        acceleration.y = msg.getArgAsFloat(i);
                        break;
                    case 6:
                        acceleration.z = msg.getArgAsFloat(i);
                        break;
                    default:
                        break;
                }
            } else if (type == "b") {
                getStatus().calibration = msg.getArgAsBlob(i);
            } else if (type == "t") {
                timetag = time.getTimeMillis();
            }
        }
        addFrame(timetag, time.getTimeMillis(), acceleration, orientation);
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
    while (count < _frames.size() && time_received - _frames[count].time > _bufferTimeMillis) {
        count++;
    }
    if (count > 0) {
        _frames.erase(_frames.begin(), _frames.begin() + count);
    }
    update();
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

sensor_status_t Sensor::getStatus() {
    return _status;
}
