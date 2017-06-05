//
// Created by anton on 04/06/17.
//

#include "MidiOut.h"

//--------------------------------------------------------------
MidiOut::MidiOut() {
    _midi = new ofxMidiOut();
    _midi->listPorts();
}

//--------------------------------------------------------------
void MidiOut::openPort(uint8_t port) {
    _midi->openPort(port);
}

//--------------------------------------------------------------
void MidiOut::openPort(string port) {
    _midi->openPort(port);
}

//--------------------------------------------------------------
vector<string> MidiOut::getPorts() {
    return _midi->portNames;
}

//--------------------------------------------------------------
void MidiOut::openVirtualPort(string port) {
    _midi->openVirtualPort(port);
}

//--------------------------------------------------------------
void MidiOut::noteOn(uint8_t channel, NoteEvent note) {
    _midi->sendNoteOn(channel, note.getMidiPitch(), note.getMidiVelocity());
}

//--------------------------------------------------------------
void MidiOut::noteOff(uint8_t channel, NoteEvent note) {
    _midi->sendNoteOff(channel, note.getMidiPitch(), 0);
}

//--------------------------------------------------------------
void MidiOut::sendController(uint8_t channel, ControllerEvent event) {
    _midi->sendControlChange(channel, event.getController(), event.getMidiValue());
}
