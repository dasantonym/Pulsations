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
    return _midi->getPortList();
}

//--------------------------------------------------------------
void MidiOut::openVirtualPort(string port) {
    _midi->openVirtualPort(port);
}

//--------------------------------------------------------------
void MidiOut::noteOn(uint8_t channel, NoteEvent note) {
    _midi->sendNoteOn((int) channel, (int) note.getMidiPitch(), (int) note.getMidiVelocity());
}

//--------------------------------------------------------------
void MidiOut::noteOff(uint8_t channel, NoteEvent note) {
    _midi->sendNoteOff((int) channel, (int) note.getMidiPitch(), 0);
}