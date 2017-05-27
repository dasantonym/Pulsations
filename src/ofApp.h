#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxDatGui.h"

#ifdef USE_VIDEO
#include "ofxVideoRecorder.h"
#endif

struct sensor_settings_t {
    bool active;
    float accelerationThreshold;
};

struct sensor_frame_t {
    uint64_t time;
    vector<float> data;
    ofBuffer calibration;
};

struct sensor_source_t {
    string id;
    string type;
    string name;
    uint64_t startTime;
    uint64_t lastTime;
    vector<ofPath> paths;
    vector<sensor_frame_t> frames;
    sensor_settings_t settings;
};

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void windowResized(int w, int h);
    void exit();

    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onTextInputEvent(ofxDatGuiTextInputEvent e);

#ifdef USE_VIDEO
    ofxVideoRecorder videoRecorder;
    ofVideoGrabber videoGrabber;
#endif

    ofxXmlSettings settings;
    ofxDatGui* gui;
    ofxOscSender sender;
    ofxOscReceiver receiver;
    ofxOscMessage filteredMessage;

    vector<sensor_source_t> sources;
    string tstamp;
    uint64_t recordingStart;
    uint64_t recordingStartMicros;
    bool isRecording;
    bool drawCurves;
};
