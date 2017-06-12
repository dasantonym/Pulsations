//
// Created by anton on 05/06/17.
//

#ifndef PULSATIONS_NOTEGENERATOR_H
#define PULSATIONS_NOTEGENERATOR_H

#include "ofMain.h"

#include "Layout.h"
#include "Types.h"
#include "NoteLoop.h"

class NoteGenerator {
public:
    NoteGenerator();

    vector<NoteEvent> evaluateTriggerResult(sensor_trigger_result_3f_t triggerResult);

private:
    TimeKeeper _time;
    ofxXmlSettings settings;
    Layout layout;
};


#endif //PULSATIONS_NOTEGENERATOR_H
