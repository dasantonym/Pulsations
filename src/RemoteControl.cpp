//
// Created by anton on 04/06/17.
//

#include "RemoteControl.h"

RemoteControl::RemoteControl() {

}

remote_command_t RemoteControl::parseCommand(ofxOscMessage msg) {
    remote_command_t command, response;
    response.command = COM_UNKNOWN;
    vector<string> addr = ofSplitString(msg.getAddress(), "/", true, true);
    if (addr[0] == "com" && addr.size() == 2) {
        command.command = (uint8_t) ofToInt(addr[1]);
        if (msg.getArgTypeName(0) == "T") {
            command.boolValue = msg.getArgAsBool(0);
        } else if (msg.getArgTypeName(0) == "f") {
            command.floatValue = msg.getArgAsFloat(0);
        }

        switch (command.command) {
            case COM_TOGGLE_RECORD_LOOP:
                response.command = command.command;
                response.boolValue = command.boolValue;
                break;
            case COM_TOGGLE_OVERDUB:
                response.command = command.command;
                response.boolValue = command.boolValue;
                break;
            default:
                break;
        }
    }
    return response;
}