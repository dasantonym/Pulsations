#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxOscSender.h"
#include "ofxOscReceiver.h"
#include "ofxXmlSettings.h"

#include "Version.h"
#include "Layout.h"
#include "Sensor.h"
#include "DataTriggers.h"
#include "MidiPlayback.h"
#include "NoteGenerator.h"
#include "RemoteControl.h"
#include "OscSerial.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void toggleLoop(bool loop);
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
    OscSerial *oscSerial;

    TimeKeeper time;
    DataTriggers *dataTrigger;
    MidiPlayback *midiPlayback;
    MidiOut *midiOut;
    NoteGenerator *noteGenerator;
    RemoteControl _remoteControl;

    vector<sensor_trigger_3d_t*> _triggers;

    vector<sensor_source_t> sources;
    vector<Sensor *> sensors;
    vector<NoteLoop> _loops;
    vector<NoteEvent> _openNotes;

    bool _drawCurves;
    bool _isRecordingLoop;
    bool _overdub;
    
    uint64_t frameCount;
};
