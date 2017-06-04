//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_MIDIOUT_H
#define PULSATIONS_MIDIOUT_H

#include "ofMain.h"
#include "ofxMidi.h"
#include "NoteEvent.h"

class MidiOut {
public:
    MidiOut();

    void openPort(uint8_t port);
    void openPort(string port);
    void openVirtualPort(string port);
    void noteOn(uint8_t channel, NoteEvent note);
    void noteOff(uint8_t channel, NoteEvent note);

private:
    ofxMidiOut _midi;
};


#endif //PULSATIONS_MIDIOUT_H
