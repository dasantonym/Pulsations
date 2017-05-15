#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "LibloDumpFile.h"

#ifdef USE_VIDEO
#include "ofxVideoRecorder.h"
#endif

struct sensor_frame_t {
    uint64_t time;
    vector<float> data;
    ofBuffer calibration;
};

struct sensor_source_t {
    string id;
    string type;
    uint64_t startTime;
    uint64_t lastTime;
    vector<ofPath> paths;
    vector<sensor_frame_t> frames;
};

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    LibloDumpFile libloFile;

#ifdef USE_VIDEO
    ofxVideoRecorder videoRecorder;
    ofVideoGrabber videoGrabber;
#endif

    ofxOscSender sender;
    ofxOscReceiver receiver;

    vector<sensor_source_t> sources;
    string tstamp;
};
