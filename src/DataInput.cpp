//
// Created by anton on 05/06/17.
//

#include "DataInput.h"

DataInput::DataInput(uint32_t udpPort) {
    _oscInput.setup(udpPort);
}

Sensor* DataInput::addBNO055Source(string id, string name = "BNO055 IMU Fusion Sensor", string type = "bno055", uint8_t index = 0) {
    Sensor *source = new Sensor(id, name, type, index);
    source->setBufferSizeMillis(1000);
    lock();
    _sources.push_back(source);
    unlock();
    return source;
}

uint64_t DataInput::sourceCount() {
    lock();
    uint64_t size = _sources.size();
    unlock();
    return size;
}

bool DataInput::sourceHasFrames(uint16_t index) {
    lock();
    bool hasFrames = index < _sources.size() && _sources[index]->hasFrames();
    unlock();
    return hasFrames;
}

sensor_frame_t DataInput::getNextFrameFromSource(uint16_t index) {
    lock();
    sensor_frame_t frame = _sources[index]->getCurrentFrame();
    unlock();
    return frame;
}

void DataInput::threadedFunction() {
    while(isThreadRunning()) {
        while (_oscInput.hasWaitingMessages()) {
            ofxOscMessage msg;
            _oscInput.getNextMessage(msg);
            vector<string> address = ofSplitString(msg.getAddress(), "/", true, true);
            if (address.size() == 2 && address[0] == "bno055") {
                lock();
                for (Sensor * sensor : _sources) {
                    if (!sensor->getStatus().active) {
                        continue;
                    }
                    sensor->parseOSCMessage(msg);
                    sensor->update();
                }
                unlock();
            }
        }
        milliseconds slt(1);
        std::this_thread::sleep_for(slt);
    }
}

void DataInput::draw() {
    lock();
    for (Sensor * sensor : _sources) {
        if (!sensor->getStatus().active) {
            continue;
        }
        sensor->draw();
    }
    unlock();
}