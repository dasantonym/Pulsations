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
    Trigger3D(ofVec3f threshold);
    Trigger3D(float threshold_low, float threshold_high);
    Trigger3D(ofVec3f threshold_low, ofVec3f threshold_high);

    void update();
    void update(ofVec3f value);

    void setTrigger(float threshold);
    void setTrigger(ofVec3f threshold);
    void setTrigger(float lowThreshold, float highThreshold);
    void setTrigger(ofVec3f lowThreshold, ofVec3f highThreshold);

    void setFalloff(float falloff);
    void setFalloff(ofVec3f falloff);

    void setDebounce(float millis);
    void setDebounce(ofVec3f millis);
    void setMask(float mask);
    void setMask(ofVec4f mask);
    void setAbsolute(bool isAbsolute);
    void setSensorInfo(uint8_t index);

    bool isTriggered();
    sensor_trigger_t* getMeta();
    ofVec3f getTrigger();
    ofVec3f getDebounceStatus();
    string getTriggerAsString();
    uint8_t getSensorIndex();
    float getTriggerMax();
    float getTriggerMin();
    float getTriggerValue(float value, float threshold_high, float threshold_low, float mask);

    sensor_trigger_result_3f_t getTriggerResult();

private:
    bool _isRange;
    bool _isAbsolute;

    bool _isActive;

    uint8_t _sensor_index;
    uint64_t _lastUpdate;

    ofVec3f _triggerValueLastMax;
    ofVec3f _triggerValue;
    ofVec3f _triggerFalloff;
    trigger_time_3d_t _lastTriggerTime;

    ofVec3f _trigger_debounce;
    ofVec3f _trigger_mask;
    ofVec3f _threshold_low;
    ofVec3f _threshold_high;

    sensor_trigger_t _meta;
    TimeKeeper time;
};


#endif //PULSATIONS_TRIGGER3D_H
