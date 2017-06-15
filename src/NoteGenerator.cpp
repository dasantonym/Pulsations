//
// Created by anton on 05/06/17.
//

#include "NoteGenerator.h"

NoteGenerator::NoteGenerator() {
    settings.load("settings.xml");
    layout.load(settings.getValue("layout", "empty"));
}

vector<NoteEvent> NoteGenerator::evaluateTriggerResult(sensor_trigger_result_3f_t triggerResult) {
    vector<NoteEvent> notes;
    string sensorPath = "layout:sensor" + triggerResult.trigger.sensor_id;
    if (triggerResult.isTriggered) {
        string triggerPath = sensorPath + ":trigger" + ofToString(triggerResult.trigger.trigger_id + 1);
        ofVec3f val = triggerResult.triggerValue;

        if (val.x != 0.f && triggerResult.debounceStatus.x == 1.f) {
            NoteEvent noteX = NoteEvent(
                    _time.getTimeMillis(),
                    (uint64_t) roundf(fabs(val.x) * (float) (uint64_t)layout.get().getValue(triggerPath + ":midi:x:duration", 100)),
                    (float) layout.get().getValue(triggerPath + ":midi:x:pitch", .5f),
                    0.5f + fabs(val.x) * (float) layout.get().getValue(triggerPath + ":midi:x:velocity", .1f),
                    (uint8_t) layout.get().getValue(triggerPath + ":midi:x:channel", 1),
                    triggerResult.trigger.name,
                    (bool) layout.get().getValue(triggerPath + ":absolute", 1) ?
                            fabs(triggerResult.triggerValue.x) : triggerResult.triggerValue.x
            );
            notes.push_back(noteX);
        }

        if (val.y != 0.f && triggerResult.debounceStatus.y == 1.f) {
            NoteEvent noteY = NoteEvent(
                    _time.getTimeMillis(),
                    (uint64_t) roundf(fabs(val.y) * (float) (uint64_t)layout.get().getValue(triggerPath + ":midi:y:duration", 100)),
                    (float) layout.get().getValue(triggerPath + ":midi:y:pitch", .5f),
                    0.5f + fabs(val.y) * (float) layout.get().getValue(triggerPath + ":midi:y:velocity", .1f),
                    (uint8_t) layout.get().getValue(triggerPath + ":midi:y:channel", 1),
                    triggerResult.trigger.name,
                    (bool) layout.get().getValue(triggerPath + ":absolute", 1) ?
                            fabs(triggerResult.triggerValue.y) : triggerResult.triggerValue.y
            );
            notes.push_back(noteY);
        }

        if (val.z != 0.f && triggerResult.debounceStatus.z == 1.f) {
            NoteEvent noteZ = NoteEvent(
                    _time.getTimeMillis(),
                    (uint64_t) roundf(fabs(val.z) * (float) (uint64_t)layout.get().getValue(triggerPath + ":midi:z:duration", 100)),
                    (float) layout.get().getValue(triggerPath + ":midi:z:pitch", .5f),
                    0.5f + fabs(val.z) * (float) layout.get().getValue(triggerPath + ":midi:z:velocity", .1f),
                    (uint8_t) layout.get().getValue(triggerPath + ":midi:z:channel", 1),
                    triggerResult.trigger.name,
                    (bool) layout.get().getValue(triggerPath + ":absolute", 1) ?
                            fabs(triggerResult.triggerValue.z) : triggerResult.triggerValue.z
            );
            notes.push_back(noteZ);
        }

        return notes;
    }
}