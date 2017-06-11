//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_TRIGGER3D_H
#define PULSATIONS_TRIGGER3D_H

#include "ofMain.h"
#include "TimeKeeper.h"
#include "Types.h"

class Trigger3D {
public:
    Trigger3D(float threshold);
    Trigger3D(ofVec4f threshold);
    Trigger3D(float threshold_low, float threshold_high);
    Trigger3D(ofVec4f threshold_low, ofVec4f threshold_high);

    void update(ofVec3f value);
    void update(ofVec4f value);

    void setTrigger(float threshold);
    void setTrigger(ofVec4f threshold);
    void setTrigger(float lowThreshold, float highThreshold);
    void setTrigger(ofVec4f lowThreshold, ofVec4f highThreshold);

    void setFalloff(float falloff);
    void setFalloff(ofVec4f falloff);

    void setDebounce(float millis);
    void setDebounce(ofVec4f millis);
    void setMask(float mask);
    void setMask(ofVec4f mask);
    void setAbsolute(bool isAbsolute);
    void setSensorInfo(uint8_t index);

    bool isTriggered();
    ofVec4f getTrigger();
    ofVec4f getDebounceStatus();
    string getTriggerAsString();
    uint8_t getSensorIndex();
    float getTriggerMax();
    float getTriggerMin();
    float getTriggerValue(float value, float threshold_high, float threshold_low, float mask);
    sensor_trigger_4d_result_t getTriggerResult();

    sensor_trigger_4d_t* trigger;
    ofFastEvent<sensor_trigger_4d_result_t> triggerValueEvent;

private:
    bool _isRange;
    bool _isAbsolute;

    bool _isActive;

    uint8_t _sensor_index;
    uint64_t _lastUpdate;

    ofVec4f _triggerValueLastMax;
    ofVec4f _triggerValue;
    ofVec4f _triggerFalloff;
    trigger_time_4d_t _lastTriggerTime;

    ofVec4f _trigger_debounce;
    ofVec4f _trigger_mask;
    ofVec4f _threshold_low;
    ofVec4f _threshold_high;

    TimeKeeper time;
};


#endif //PULSATIONS_TRIGGER3D_H
