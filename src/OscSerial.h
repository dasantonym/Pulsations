//
// Created by anton on 10/06/17.
//

#ifndef PULSATIONS_OSCSERIAL_H
#define PULSATIONS_OSCSERIAL_H

//#define RECEIVE_QUATERNION = 1;
#define RECEIVE_LINEAR_ACCELERATION = 1;
#define RECEIVE_EULER = 1;

#include "ofMain.h"
#include "ofxOscReceiver.h"
#include "ofxOscSender.h"
#include "Packets.h"
#include "TimeKeeper.h"

class OscSerial: public ofThread {
public:
    OscSerial();
    OscSerial(string forwardIp, int forwardPort);

    bool setup(string deviceName, uint32_t baud);
    bool hasFrames();
    bool hasFrames(uint8_t sensor_id);
    vector<sensor_frame_t> getFrames();
    sensor_frame_t getAverageFrame(uint8_t sensor_id, bool skipOrientation = true);
    sensor_frame_t getMaxFrame(uint8_t sensor_id, bool absolute = true, bool skipOrientation = true);
    sensor_status_t getStatus(uint8_t id);

    void threadedFunction();

    const uint8_t EOT = 192;
    const uint8_t SLIP_ESC = 219;
    const uint8_t SLIP_ESC_END = 220;
    const uint8_t SLIP_ESC_ESC = 221;

private:
    ofxOscSender _oscOut;
    ofSerial _serial;
    vector<uint8_t> _buffer;
    vector <ofSerialDeviceInfo> _deviceList;
    vector<sensor_frame_t> _frames;
    vector<vector<sensor_frame_t>> _sensorFrames;
    vector<sensor_frame_t> _avgFrame;
    vector<sensor_status_t> _status;
    osc::IpEndpointName _endpoint;

    uint32_t _packetSize;
    int _lastRead;
    bool _isReady;

    TimeKeeper _time;
};


#endif //PULSATIONS_OSCSERIAL_H
