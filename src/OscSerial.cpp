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
    lock();
    bool hasFrames = _frames.size() != 0;
    unlock();
    return hasFrames;
}

sensor_frame_t OscSerial::getNextFrame() {
    lock();
    sensor_frame_t frame = _frames[0];
    _frames.erase(_frames.begin(), _frames.begin() + 1);
    unlock();
    return frame;
}

vector<sensor_frame_t> OscSerial::getFrames() {
    vector<sensor_frame_t> frames;
    lock();
    _frames.swap(frames);
    unlock();
    return frames;
}

sensor_status_t OscSerial::getStatus(uint8_t id) {
    sensor_status_t status;
    lock();
    if (id < _status.size()) {
        status = _status[id];
    } else {
        status.calibration.allocate(4);
    }
    unlock();
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
                        //try {
                        if (_buffer.size() >= 3) {
                            if (_buffer[0] == 1 && _buffer.size() == 27) {
                                sensor_frame_t frame;
                                frame.time_received = _time.getTimeMillis();
                                frame.time = frame.time_received;
                                frame.sensor_id = ofToString((int) _buffer[1]);

                                const char *data = (const char *) _buffer.data();
                                int size = (int) data[2];
                                int i = 3;
                                float *fl;

                                fl = (float *) &data[i];
                                frame.orientation.x = *fl;
                                i += 4;//sizeof(float);
                                fl = (float *) &data[i];
                                frame.orientation.y = *fl;
                                i += 4;
                                fl = (float *) &data[i];
                                frame.orientation.z = *fl;
                                i += 4;

                                fl = (float *) &data[i];
                                frame.acceleration.x = *fl;
                                i += 4;
                                fl = (float *) &data[i];
                                frame.acceleration.y = *fl;
                                i += 4;
                                fl = (float *) &data[i];
                                frame.acceleration.z = *fl;
                                i += 4;

                                if (size == 27) {
                                    lock();
                                    _frames.push_back(frame);
                                    unlock();
                                }
                            } else if (_buffer[0] == 2 && _buffer.size() == 7) {
                                const char *data = (const char *) _buffer.data();
                                int size = (int) data[2];
                                if (size == 7) {
                                    uint8_t id = (uint8_t)data[1];
                                    while (id >= _status.size()) {
                                        sensor_status_t status;
                                        status.active = true;
                                        status.calibration.allocate(4);
                                        _status.push_back(status);
                                    }
                                    _status[id].calibration.getData()[0] = data[3];
                                    _status[id].calibration.getData()[1] = data[4];
                                    _status[id].calibration.getData()[2] = data[5];
                                    _status[id].calibration.getData()[3] = data[6];
                                }
                            }
                            //_osc.ProcessPacket((const char *) _buffer.data(), (int) _buffer.size(), _endpoint);
                            /*
                            } catch (...) {

                            }
                        */
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
            /*
            ofxOscBundle bundle;
            while (_osc.hasWaitingMessages()) {
                ofxOscMessage msg;
                _osc.getNextMessage(msg);
                vector<string> addr = ofSplitString(msg.getAddress(), "/", true, true);
                if (addr.size() == 2 && addr[0] == "u" && msg.getNumArgs() == 7) {
                    msg.setAddress("/bno/10" + addr[1]);
                    bundle.addMessage(msg);
                    sensor_frame_t frame;
                    frame.sensor_id = addr[1];
                    frame.time = (uint64_t)msg.getArgAsTimetag(0);
                    frame.time_received = _time.getTimeMillis();
                    frame.orientation.x = msg.getArgAsFloat(1);
                    frame.orientation.y = msg.getArgAsFloat(2);
                    frame.orientation.z = msg.getArgAsFloat(3);
                    frame.acceleration.x = msg.getArgAsFloat(4);
                    frame.acceleration.y = msg.getArgAsFloat(5);
                    frame.acceleration.z = msg.getArgAsFloat(6);
                    lock();
                    _frames.push_back(frame);
                    unlock();
                }
            }
            if (bundle.getMessageCount() != 0) {
                _oscOut.sendBundle(bundle);
            }
             */
        }
        milliseconds slt(1);
        std::this_thread::sleep_for(slt);
    }
    ofLogNotice() << "OscSerial: Thread ended" << endl;
}