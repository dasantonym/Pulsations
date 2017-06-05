//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_REMOTECONTROL_H
#define PULSATIONS_REMOTECONTROL_H

#include "ofMain.h"
#include "Types.h"

class RemoteControl {
public:
    RemoteControl();

    remote_command_t parseCommand(ofxOscMessage msg);

    static const uint8_t COM_TOGGLE_RECORD_LOOP = 0;
    static const uint8_t COM_TOGGLE_OVERDUB = 1;

    static const uint8_t COM_UNKNOWN = 255;
};


#endif //PULSATIONS_REMOTECONTROL_H
