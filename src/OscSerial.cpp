//
// Created by anton on 10/06/17.
//

#include "OscSerial.h"

OscSerial::OscSerial() {
    _isReady = false;

    _oscOut.setup("127.0.0.1", 8888);

    _endpoint.address = osc::IpEndpointName::ANY_ADDRESS;
    _endpoint.port = osc::IpEndpointName::ANY_PORT;

    _serial.listDevices();

    _packetSize = Packets::getPacketSize();
}

bool OscSerial::setup(string deviceName, uint32_t baud) {
    _deviceList = _serial.getDeviceList();

    for (ofSerialDeviceInfo & device : _deviceList) {
        if (device.getDeviceName() == deviceName) {
            _isReady = _serial.setup(deviceName, baud);
        }
    }

    return _isReady;
}

bool OscSerial::hasFrames() {
    return _frames.size() != 0;
}

bool OscSerial::hasFrames(uint8_t sensor_id) {
    return sensor_id < _sensorFrames.size() && _sensorFrames[sensor_id].size();
}

vector<sensor_frame_t> OscSerial::getFrames() {
    vector<sensor_frame_t> frames;
    lock();
    _frames.swap(frames);
    unlock();
    return frames;
}

sensor_frame_t OscSerial::getAverageFrame(uint8_t sensor_id, bool skipOrientation) {
    sensor_frame_t avgFrame;
    avgFrame.sensor_id = ofToString(sensor_id + 1);
    avgFrame.acceleration = { .0f, .0f, .0f };
    avgFrame.orientation = { .0f, .0f, .0f };
    uint64_t count = 0;
    for (sensor_frame_t & frame : _sensorFrames[sensor_id]) {
        avgFrame.acceleration.x += frame.acceleration.x;
        avgFrame.acceleration.y += frame.acceleration.y;
        avgFrame.acceleration.z += frame.acceleration.z;
        if (skipOrientation) {
            avgFrame.orientation = frame.orientation;
        } else {
            avgFrame.orientation.x += frame.orientation.x;
            avgFrame.orientation.y += frame.orientation.y;
            avgFrame.orientation.z += frame.orientation.z;
        }
        count++;
    }
    if (count) {
        avgFrame.acceleration.x /= count;
        avgFrame.acceleration.y /= count;
        avgFrame.acceleration.z /= count;
        if (!skipOrientation) {
            avgFrame.orientation.x /= count;
            avgFrame.orientation.y /= count;
            avgFrame.orientation.z /= count;
        }
        lock();
        _sensorFrames[sensor_id].clear();
        unlock();
    }

    avgFrame.time = _time.getTimeMillis();
    avgFrame.time_received = avgFrame.time;
    return avgFrame;
}

sensor_frame_t OscSerial::getMaxFrame(uint8_t sensor_id, bool absolute, bool skipOrientation) {
    sensor_frame_t maxFrame;
    maxFrame.sensor_id = ofToString(sensor_id + 1);
    maxFrame.acceleration = { .0f, .0f, .0f };
    maxFrame.orientation = { .0f, .0f, .0f };
    for (sensor_frame_t & frame : _sensorFrames[sensor_id]) {
        float val, max;

        val = absolute ? fabs(frame.acceleration.x) : frame.acceleration.x;
        max = absolute ? fabs(maxFrame.acceleration.x) : maxFrame.acceleration.x;
        if (val > max) maxFrame.acceleration.x = frame.acceleration.x;
        val = absolute ? fabs(frame.acceleration.y) : frame.acceleration.y;
        max = absolute ? fabs(maxFrame.acceleration.y) : maxFrame.acceleration.y;
        if (val > max) maxFrame.acceleration.y = frame.acceleration.y;
        val = absolute ? fabs(frame.acceleration.z) : frame.acceleration.z;
        max = absolute ? fabs(maxFrame.acceleration.z) : maxFrame.acceleration.z;
        if (val > max) maxFrame.acceleration.z = frame.acceleration.z;

        if (skipOrientation) {
            maxFrame.orientation = frame.orientation;
        } else {
            val = absolute ? fabs(frame.orientation.x) : frame.orientation.x;
            max = absolute ? fabs(maxFrame.orientation.x) : maxFrame.orientation.x;
            if (val > max) maxFrame.orientation.x = frame.orientation.x;
            val = absolute ? fabs(frame.orientation.y) : frame.orientation.y;
            max = absolute ? fabs(maxFrame.orientation.y) : maxFrame.orientation.y;
            if (val > max) maxFrame.orientation.y = frame.orientation.y;
            val = absolute ? fabs(frame.orientation.z) : frame.orientation.z;
            max = absolute ? fabs(maxFrame.orientation.z) : maxFrame.orientation.z;
            if (val > max) maxFrame.orientation.z = frame.orientation.z;
        }
    }
    if (_sensorFrames[sensor_id].size()) {
        lock();
        _sensorFrames[sensor_id].clear();
        unlock();
    }
    maxFrame.time = _time.getTimeMillis();
    maxFrame.time_received = maxFrame.time;
    return maxFrame;
}

sensor_status_t OscSerial::getStatus(uint8_t id) {
    sensor_status_t status;
    if (id < _status.size()) {
        lock();
        status = _status[id];
        unlock();
    } else {
        lock();
        status.calibration.allocate(4);
        status.system.allocate(3);
        unlock();
    }
    return status;
}

void OscSerial::threadedFunction() {
    while (isThreadRunning()) {
        if (_isReady) {
            while (_serial.available()) {
                int read = _serial.readByte();
                if (read == OF_SERIAL_ERROR) {
                    ofLogNotice() << "Error reading serial";
                } else if (read != OF_SERIAL_NO_DATA) {
                    if (read == EOT) {
                        if (_buffer.size() >= 3) {
                            if (_buffer[0] == 1 && _buffer.size() == _packetSize) {
                                ofxOscMessage msg;
                                msg.setAddress("/bno055/10" + ofToString((int)_buffer[1] - 1));

                                sensor_frame_t frame;
                                frame.sensor_id = ofToString((int) _buffer[1]);

                                frame.time_received = _time.getTimeMillis();
                                frame.time = frame.time_received;
                                msg.addTimetagArg(frame.time);

                                const char *data = (const char *) _buffer.data();
                                int size = (int) data[2];

                                if (size == _packetSize) {
                                    int i = 3;
#ifdef RECEIVE_QUATERNION
                                    i = Packets::unpackFloat(frame.quaternion.x, data, i);
                                    msg.addFloatArg(frame.quaternion.x);
                                    i = Packets::unpackFloat(frame.quaternion.y, data, i);
                                    msg.addFloatArg(frame.quaternion.y);
                                    i = Packets::unpackFloat(frame.quaternion.z, data, i);
                                    msg.addFloatArg(frame.quaternion.z);
                                    i = Packets::unpackFloat(frame.quaternion.w, data, i);
                                    msg.addFloatArg(frame.quaternion.w);
#endif

#ifdef RECEIVE_EULER
                                    i = Packets::unpackFloat(frame.orientation.x, data, i);
                                    msg.addFloatArg(frame.orientation.x);
                                    i = Packets::unpackFloat(frame.orientation.y, data, i);
                                    msg.addFloatArg(frame.orientation.y);
                                    i = Packets::unpackFloat(frame.orientation.z, data, i);
                                    msg.addFloatArg(frame.orientation.z);
#endif

#ifdef RECEIVE_LINEAR_ACCELERATION
                                    i = Packets::unpackFloat(frame.acceleration.x, data, i);
                                    //frame.acceleration.x *= ofSign(frame.orientation.x - 180.f);
                                    msg.addFloatArg(frame.acceleration.x);
                                    i = Packets::unpackFloat(frame.acceleration.y, data, i);
                                    //frame.acceleration.y *= ofSign(frame.orientation.y);
                                    msg.addFloatArg(frame.acceleration.y);
                                    i = Packets::unpackFloat(frame.acceleration.z, data, i);
                                    //frame.acceleration.z *= ofSign(frame.orientation.z);
                                    msg.addFloatArg(frame.acceleration.z);
#endif

                                    _oscOut.sendMessage(msg, true);

                                    lock();
                                    _frames.push_back(frame);
                                    unlock();

                                    int id = (uint8_t)data[1] - 1;
                                    while (id >= _sensorFrames.size()) {
                                        vector<sensor_frame_t> frames;
                                        lock();
                                        _sensorFrames.push_back(frames);
                                        unlock();
                                    }

                                    if (id >= 0 && id < _sensorFrames.size()) {
                                        lock();
                                        _sensorFrames[id].push_back(frame);
                                        unlock();
                                    }
                                } else {
                                    ofLogNotice() << "Malformed packet size" << endl;
                                }
                            } else if (_buffer[0] == 2 && _buffer.size() == 7) {
                                const char *data = (const char *) _buffer.data();
                                int size = (int) data[2];
                                if (size == 7) {
                                    uint8_t id = (uint8_t)data[1];
                                    lock();
                                    while (id >= _status.size()) {
                                        sensor_status_t status;
                                        status.active = true;
                                        status.calibration.allocate(4);
                                        status.system.allocate(3);
                                        _status.push_back(status);
                                    }
                                    unlock();

                                    lock();
                                    _status[id].calibration.getData()[0] = data[3];
                                    _status[id].calibration.getData()[1] = data[4];
                                    _status[id].calibration.getData()[2] = data[5];
                                    _status[id].calibration.getData()[3] = data[6];
                                    unlock();
                                }
                            } else if (_buffer[0] == 3 && _buffer.size() == 6) {
                                const char *data = (const char *) _buffer.data();
                                int size = (int) data[2];
                                if (size == 6) {
                                    uint8_t id = (uint8_t)data[1];
                                    while (id >= _status.size()) {
                                        sensor_status_t status;
                                        status.active = true;
                                        status.calibration.allocate(4);
                                        status.system.allocate(3);
                                        lock();
                                        _status.push_back(status);
                                        unlock();
                                    }

                                    lock();
                                    _status[id].system.getData()[0] = data[3];
                                    _status[id].system.getData()[1] = data[4];
                                    _status[id].system.getData()[2] = data[5];
                                    unlock();
                                }
                            }
                        }
                        _buffer.clear();
                    } else if (_lastRead == SLIP_ESC) {
                        if (read == SLIP_ESC_END) {
                            _buffer.push_back(EOT);
                        } else if (read == SLIP_ESC_ESC) {
                            _buffer.push_back(SLIP_ESC);
                        }
                    } else {
                        _buffer.push_back((uint8_t)read);
                    }
                    _lastRead = read;
                }
            }
        }
        /*
        milliseconds slt(1);
        std::this_thread::sleep_for(slt);
         */
    }
    ofLogNotice() << "OscSerial: Thread ended" << endl;
}