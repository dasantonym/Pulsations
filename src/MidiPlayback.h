//
// Created by anton on 05/06/17.
//

#ifndef PULSATIONS_MIDIPLAYBACK_H
#define PULSATIONS_MIDIPLAYBACK_H

#include "ofMain.h"
#include "TimeKeeper.h"
#include "MidiOut.h"
#include "ofxOscSender.h"

class MidiPlayback : public ofThread {
public:
    MidiPlayback();
    MidiPlayback(string oscOutputIp, int oscOutputPort);

    void setMidi(uint8_t port);
    void addNote(NoteEvent inputNote);
    void threadedFunction();
    vector<string> getPorts();

private:
    MidiOut* _midi;
    ofxOscSender _osc;
    TimeKeeper _time;
    vector<NoteEvent> _notes;
};


#endif //PULSATIONS_MIDIPLAYBACK_H
