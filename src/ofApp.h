#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxDatGui.h"

#include "Sensor.h"
#include "NoteLoop.h"
#include "Layout.h"

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
    ofxOscReceiver receiver;

    MidiOut *midiOut;

    vector<sensor_source_t> sources;
    vector<Sensor> sensors;
    vector<NoteLoop> _loops;
    vector<NoteEvent> _openNotes;

    bool _drawCurves;
    bool _isRecordingLoop;
};
