//
// Created by anton on 05/06/17.
//

#include "DataTriggers.h"

DataTriggers::DataTriggers() {

}

//
//
// Adding triggers
//

Trigger3D* DataTriggers::addTrigger(string name, string target, string targetSensorId, float threshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(threshold);
    trigger->trigger->name = name;
    trigger->trigger->target = target;
    trigger->trigger->target_sid = targetSensorId;
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* DataTriggers::addTrigger(string name, string target, string targetSensorId, float lowThreshold, float highThreshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger->trigger->name = name;
    trigger->trigger->target = target;
    trigger->trigger->target_sid = targetSensorId;
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* DataTriggers::addTrigger(string name, string target, string targetSensorId, ofVec3f threshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(threshold);
    trigger->trigger->name = name;
    trigger->trigger->target = target;
    trigger->trigger->target_sid = targetSensorId;
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

Trigger3D* DataTriggers::addTrigger(string name, string target, string targetSensorId, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute = false) {
    Trigger3D* trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger->trigger->name = name;
    trigger->trigger->target = target;
    trigger->trigger->target_sid = targetSensorId;
    trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}


void DataTriggers::addFrame(sensor_frame_t inputFrame) {
    sensor_frame_t frame;
    frame.orientation = inputFrame.orientation;
    frame.acceleration = inputFrame.acceleration;
    frame.time_received = inputFrame.time_received;
    frame.time = inputFrame.time;
    frame.data = inputFrame.data;
    frame.sensor_id = inputFrame.sensor_id;
    lock();
    _frames.push_back(frame);
    unlock();
}

void DataTriggers::threadedFunction() {
    while (isThreadRunning()) {
        lock();
        while (_frames.size() > 0) {
            for (Trigger3D *trigger : _triggers) {
                if (trigger->trigger->target == "acceleration" && trigger->trigger->target_sid == _frames[0].sensor_id) {
                    trigger->update(_frames[0].acceleration);
                }
                if (trigger->trigger->target == "orientation" && trigger->trigger->target_sid == _frames[0].sensor_id) {
                    trigger->update(_frames[0].orientation);
                }
            }
            _frames.erase(_frames.begin(), _frames.begin() + 1);
        }
        unlock();
        milliseconds slt(1);
        std::this_thread::sleep_for(slt);
    }
    ofLogNotice() << "DataTriggers exited" << endl;
}

void DataTriggers::draw() {
    uint32_t tc = 0;
    lock();
    for (Trigger3D *trigger : _triggers) {
        if (trigger->getSensorIndex() > 0) {
            ofDrawBitmapString(trigger->trigger->name + " " + trigger->getTriggerAsString() + "   ",
                    40.f, 40.f + 160.f * (tc + 1) - 140.f);
        }
        tc++;
    }
    unlock();
}

int DataTriggers::getTriggerIndexByName(string name) {
    int result = -1;
    int count = 0;
    for (Trigger3D *trigger : _triggers) {
        if (trigger->trigger->name == name) {
            result = count;
            return result;
        }
        count++;
    }
    return result;
}

vector<sensor_trigger_3d_result_t> DataTriggers::getAllTriggerResults() {
    vector<sensor_trigger_3d_result_t> results;
    uint8_t count = 0;

    for (Trigger3D *trigger : _triggers) {
        sensor_trigger_3d_result_t res = getTriggerResult(count);
        if (res.isTriggered) {
            results.push_back(res);
        }
        count++;
    }

    return results;
}

sensor_trigger_3d_result_t DataTriggers::getTriggerResult(uint8_t index) {
    sensor_trigger_3d_result_t res;
    Trigger3D *trigger = _triggers[index];
    if (trigger->isTriggered()) {
        res.triggerValue = trigger->getTrigger();
        res.index = index;
        res.target_sid = trigger->trigger->target_sid;
        res.target = trigger->trigger->target;
        res.name = trigger->trigger->name;
        res.isTriggered = trigger->isTriggered();
        res.debounceStatus = trigger->getDebounceStatus();
        res.sensor_index = trigger->getSensorIndex();
    }
    return res;
}