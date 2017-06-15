#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxOscSender.h"
#include "ofxOscReceiver.h"
#include "ofxXmlSettings.h"

#include "Version.h"
#include "Layout.h"
#include "Sensor.h"
#include "MidiPlayback.h"
#include "NoteGenerator.h"
#include "OscSerial.h"

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

    ofxXmlSettings settings;
    Layout layout;
    ofxDatGui* gui;
    ofxOscSender sender;
    ofxOscSender uiResponder;
    ofxOscReceiver receiver;
    OscSerial *oscSerialA;
    OscSerial *oscSerialB;

    TimeKeeper time;
    MidiPlayback *midiPlayback;
    NoteGenerator *noteGenerator;
    vector<Sensor *> sensors;

    bool _drawGraph;
    bool _drawGui;
    
    uint64_t frameCount;
};
