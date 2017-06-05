//
// Created by anton on 05/06/17.
//

#ifndef PULSATIONS_NOTEGENERATOR_H
#define PULSATIONS_NOTEGENERATOR_H

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "Layout.h"

#include "Types.h"
#include "NoteLoop.h"

class NoteGenerator {
public:
    NoteGenerator();

    vector<NoteEvent> evaluateTriggerResult(sensor_trigger_3d_result_t triggerResult, bool sendOsc);

private:
    TimeKeeper _time;
    ofxOscSender _sender;
    ofxXmlSettings settings;
    Layout layout;
};


#endif //PULSATIONS_NOTEGENERATOR_H
