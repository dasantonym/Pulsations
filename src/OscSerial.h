//
// Created by anton on 10/06/17.
//

#ifndef PULSATIONS_OSCSERIAL_H
#define PULSATIONS_OSCSERIAL_H

#include "ofMain.h"
#include "ofxIO.h"
#include "ofxUDPManager.h"
#include "ofxOscReceiver.h"
#include "ofxOscSender.h"
#include "Types.h"
#include "TimeKeeper.h"

class OscSerial: public ofThread {
public:
    OscSerial();

    bool setup(string deviceName, uint32_t baud);
    bool hasFrames();
    sensor_frame_t getNextFrame();
    vector<sensor_frame_t> getFrames();
    sensor_status_t getStatus(uint8_t id);

    void threadedFunction();

    const uint8_t EOT = 192;
    const uint8_t SLIP_ESC = 219;
    const uint8_t SLIP_ESC_END = 220;
    const uint8_t SLIP_ESC_ESC = 221;

    static const uint16_t KEEP_FRAMES = 500;

private:
    ofxOscReceiver _osc;
    ofxOscSender _oscOut;
    ofxUDPManager _udp;
    ofSerial _serial;
    vector<uint8_t> _buffer;
    vector <ofSerialDeviceInfo> _deviceList;
    vector<sensor_frame_t> _frames;
    vector<sensor_status_t> _status;
    osc::IpEndpointName _endpoint;
    int _lastRead;
    bool _isReady;

    TimeKeeper _time;
};


#endif //PULSATIONS_OSCSERIAL_H
