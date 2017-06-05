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

sensor_trigger_3d_t DataTriggers::addTrigger(string name, string target, string targetSensorId, float threshold, bool absolute = false) {
    sensor_trigger_3d_t trigger;
    trigger.name = name;
    trigger.target = target;
    trigger.target_sid = targetSensorId;
    trigger.trigger = new Trigger3D(threshold);
    trigger.trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

sensor_trigger_3d_t DataTriggers::addTrigger(string name, string target, string targetSensorId, float lowThreshold, float highThreshold, bool absolute = false) {
    sensor_trigger_3d_t trigger;
    trigger.name = name;
    trigger.target = target;
    trigger.target_sid = targetSensorId;
    trigger.trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger.trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

sensor_trigger_3d_t DataTriggers::addTrigger(string name, string target, string targetSensorId, ofVec3f threshold, bool absolute = false) {
    sensor_trigger_3d_t trigger;
    trigger.name = name;
    trigger.target = target;
    trigger.target_sid = targetSensorId;
    trigger.trigger = new Trigger3D(threshold);
    trigger.trigger->setAbsolute(absolute);
    _triggers.push_back(trigger);
    return trigger;
}

sensor_trigger_3d_t DataTriggers::addTrigger(string name, string target, string targetSensorId, ofVec3f lowThreshold, ofVec3f highThreshold, bool absolute = false) {
    sensor_trigger_3d_t trigger;
    trigger.name = name;
    trigger.target = target;
    trigger.target_sid = targetSensorId;
    trigger.trigger = new Trigger3D(lowThreshold, highThreshold);
    trigger.trigger->setAbsolute(absolute);
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
    lock();
    _frames.push_back(frame);
    unlock();
}

void DataTriggers::threadedFunction() {
    while (isThreadRunning()) {
        while (_frames.size() > 0) {
            for (sensor_trigger_3d_t &trigger : _triggers) {
                if (trigger.target == "acceleration") {
                    lock();
                    trigger.trigger->update(_frames[0].acceleration);
                    unlock();
                }
                if (trigger.target == "orientation") {
                    lock();
                    trigger.trigger->update(_frames[0].orientation);
                    unlock();
                }
            }
            _frames.erase(_frames.begin(), _frames.begin() + 1);
        }
        milliseconds slt(1);
        std::this_thread::sleep_for(slt);
    }
}

int DataTriggers::getTriggerIndexByName(string name) {
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

vector<sensor_trigger_3d_result_t> DataTriggers::getAllTriggerResults() {
    vector<sensor_trigger_3d_result_t> results;
    uint8_t count = 0;

    for (sensor_trigger_3d_t & trigger : _triggers) {
        if (trigger.trigger->isTriggered()) {
            sensor_trigger_3d_result_t res;
            res.triggerValue = trigger.trigger->getTrigger();
            res.index = count;
            res.target_sid = trigger.target_sid;
            res.target = trigger.target;
            res.name = trigger.name;
            res.isTriggered = trigger.trigger->isTriggered();
            res.debounceStatus = trigger.trigger->getDebounceStatus();
            res.sensor_index = trigger.trigger->getSensorIndex();
        }

        count++;
    }

    return results;
}