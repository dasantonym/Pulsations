//
// Created by anton on 02/06/17.
//

#include "SensorSources.h"

void SensorSources::addIMU(string sid, float accThreshold) {
    string valuePath = "sensor:id" + ofToString(imu.size() + 1);
    sensor_source_t source;
    source.id = sid;
    source.type = "bno055";
    source.name = "BNO 055 IMU Fusion Sensor";
    source.startTime = 0;
    source.settings.active = true;
    source.settings.accelerationThreshold = accThreshold;
    for (int i = 0; i < 6; ++i) {
        ofPath path;
        path.setFilled(false);
        path.setStrokeWidth(1.0f);
        ofColor color;
        switch (i) {
            case 0:
                path.setStrokeColor(color.red);
                break;
            case 1:
                path.setStrokeColor(color.green);
                break;
            case 2:
                path.setStrokeColor(color.blue);
                break;
            case 3:
                path.setStrokeColor(color.yellow);
                break;
            case 4:
                path.setStrokeColor(color.cyan);
                break;
            case 5:
                path.setStrokeColor(color.magenta);
                break;
        }
        source.paths.push_back(path);
    }
    imu.push_back(source);
}

sensor_source_t SensorSources::getIMU(uint8_t index) {
    return imu[index];
}

bool SensorSources::parseOscFrame(ofxOscMessage &msg) {
    uint64_t frame_time = ofGetElapsedTimeMillis();
    vector<string> address = ofSplitString(msg.getAddress(), "/", true, true);
    if (address.size() == 2 && address[0] == "bno055") {
        bool sendFrame;
        ofVec4f acceleration, orientation;
        ofxOscMessage filteredMessage;
        filteredMessage.setAddress(msg.getAddress());
        filteredMessage.addTimetagArg(frame_time);
        for (sensor_source_t source : imu) {
            sensor_frame_t frame;
            if (source.type == address[0] && source.id == address[1]) {
                // FIXME: timetags are not received properly...
                sendFrame = false;
                for (int i = 0; i < msg.getNumArgs(); ++i) {
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
                        if (i > 3 && i < 7) {
                            if (fabs(msg.getArgAsFloat(i)) >= source.settings.accelerationThreshold) {
                                sendFrame = true;
                                filteredMessage.addFloatArg(msg.getArgAsFloat(i));
                            }
                            frame.data.push_back(msg.getArgAsFloat(i));
                        } else {
                            frame.data.push_back(msg.getArgAsFloat(i));
                        }
                    } else if (type == "b") {
                        frame.calibration = msg.getArgAsBlob(i);
                    } else if (type == "T" || type == "t") {
                        if (source.startTime == 0) {
                            source.startTime = frame_time;
                        }
                        orientation.w = frame_time;
                        acceleration.w = frame_time;
                        source.lastTime = frame_time;
                        frame.time = frame_time;
                    }
                }
                source.stats.accelerationValues.push_back(acceleration);
                source.stats.orientationValues.push_back(orientation);
                source.frames.push_back(frame);
                source.eventQueue.push_back(filteredMessage);
                return sendFrame;
            }
        }
    }
}