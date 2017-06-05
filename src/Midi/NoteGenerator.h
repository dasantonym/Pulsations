//
// Created by anton on 05/06/17.
//

#ifndef PULSATIONS_NOTEGENERATOR_H
#define PULSATIONS_NOTEGENERATOR_H

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "Layout.h"
#include "Types.h"
#include "NoteEvent.h"
#include "TimeKeeper.h"

class NoteGenerator {
public:
    NoteGenerator();

    vector<NoteEvent> evaluateTriggerResult(sensor_trigger_3d_result_t triggerResult);

private:
    TimeKeeper _time;
    ofxXmlSettings settings;
    Layout layout;
};


#endif //PULSATIONS_NOTEGENERATOR_H
