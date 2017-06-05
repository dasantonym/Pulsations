//
// Created by anton on 05/06/17.
//

#include "NoteGenerator.h"

NoteGenerator::NoteGenerator() {
    settings.load("settings.xml");
    layout.load(settings.getValue("layout", "empty"));
}

vector<NoteEvent> NoteGenerator::evaluateTriggerResult(sensor_trigger_3d_result_t triggerResult) {
    vector<NoteEvent> notes;
    string sensorPath = "layout:sensor" + ofToString(triggerResult.sensor_index);
    if (triggerResult.isTriggered) {
        //bool sendosc = false;
        string triggerPath = sensorPath + ":trigger" + ofToString(triggerResult.index);
        ofVec3f val = triggerResult.triggerValue;

        if (val.x > 0.f && triggerResult.debounceStatus.x == 1.f) {
            //sendosc = true;
            NoteEvent noteX = NoteEvent(
                    _time.getTimeMillis(),
                    (uint64_t)layout.get().getValue(triggerPath + ":midi:x:duration", 250),
                    (float) layout.get().getValue(triggerPath + ":midi:x:pitch", .5f),
                    (float) layout.get().getValue(triggerPath + ":midi:x:velocity", .8f),
                    (uint8_t) layout.get().getValue(triggerPath + ":midi:x:channel", 1)
            );
            notes.push_back(noteX);
            /*
            if (_isRecordingLoop) {
                _loops[_loops.size() - 1].addNote(noteX);
            }
             */
        }

        if (val.y > 0.f && triggerResult.debounceStatus.y == 1.f) {
            //sendosc = true;
            NoteEvent noteY = NoteEvent(
                    _time.getTimeMillis(),
                    (uint64_t)layout.get().getValue(triggerPath + ":midi:y:duration", 250),
                    (float) layout.get().getValue(triggerPath + ":midi:y:pitch", .5f),
                    (float) layout.get().getValue(triggerPath + ":midi:y:velocity", .8f),
                    (uint8_t) layout.get().getValue(triggerPath + ":midi:y:channel", 1)
            );
            notes.push_back(noteY);
        }

        if (val.z > 0.f && triggerResult.debounceStatus.z == 1.f) {
            //sendosc = true;
            NoteEvent noteZ = NoteEvent(
                    _time.getTimeMillis(),
                    (uint64_t)layout.get().getValue(triggerPath + ":midi:z:duration", 250),
                    (float) layout.get().getValue(triggerPath + ":midi:z:pitch", .5f),
                    (float) layout.get().getValue(triggerPath + ":midi:z:velocity", .8f),
                    (uint8_t) layout.get().getValue(triggerPath + ":midi:z:channel", 1)
            );
            notes.push_back(noteZ);
        }

        /*
        if (sendosc) {
            ofxOscMessage msgOut;
            msgOut.setAddress(sensor.getOSCAddress() + "/" + trigger->target + "/" + trigger->name);
            msgOut.addFloatArg(val.x);
            msgOut.addFloatArg(val.y);
            msgOut.addFloatArg(val.z);
            bundle.addMessage(msgOut);
        }
         */

        return notes;
    }
}