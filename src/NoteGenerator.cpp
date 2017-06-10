//
// Created by anton on 05/06/17.
//

#include "NoteGenerator.h"

NoteGenerator::NoteGenerator() {
    settings.load("settings.xml");
    layout.load(settings.getValue("layout", "empty"));

    _sender.setup(
            settings.getValue("osc:forwardIP", "127.0.0.1"),
            settings.getValue("osc:forwardPort", 9999));
}

vector<NoteEvent> NoteGenerator::evaluateTriggerResult(sensor_trigger_3d_result_t triggerResult, bool sendOsc = true) {
    vector<NoteEvent> notes;
    ofxOscBundle oscBundle;
    string sensorPath = "layout:sensor" + triggerResult.target_sid;
    if (triggerResult.isTriggered) {
        string triggerPath = sensorPath + ":trigger" + ofToString(triggerResult.trigger_index + 1);
        ofVec3f val = triggerResult.triggerValue;

        if (val.x > 0.f && triggerResult.debounceStatus.x == 1.f) {
            /*
            ofxOscMessage msgOut;
            msgOut.setAddress("/" + triggerResult.name + "/" + triggerResult.target_sid + "/x");
            msgOut.addFloatArg(val.x);
            oscBundle.addMessage(msgOut);
             */

            NoteEvent noteX = NoteEvent(
                    _time.getTimeMillis(),
                    (uint64_t)layout.get().getValue(triggerPath + ":midi:x:duration", 250),
                    (float) layout.get().getValue(triggerPath + ":midi:x:pitch", .5f),
                    (float) layout.get().getValue(triggerPath + ":midi:x:velocity", .8f),
                    (uint8_t) layout.get().getValue(triggerPath + ":midi:x:channel", 1)
            );
            notes.push_back(noteX);
        }

        if (val.y > 0.f && triggerResult.debounceStatus.y == 1.f) {
            /*
            ofxOscMessage msgOut;
            msgOut.setAddress("/" + triggerResult.name + "/" + triggerResult.target_sid + "/y");
            msgOut.addFloatArg(val.y);
            oscBundle.addMessage(msgOut);
             */

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
            /*
            ofxOscMessage msgOut;
            msgOut.setAddress("/" + triggerResult.name + "/" + triggerResult.target_sid + "/z");
            msgOut.addFloatArg(val.z);
            oscBundle.addMessage(msgOut);
             */

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
        if (sendOsc && oscBundle.getMessageCount() > 0) {
            _sender.sendBundle(oscBundle);
        }
         */

        return notes;
    }
}