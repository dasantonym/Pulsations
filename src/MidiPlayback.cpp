//
// Created by anton on 05/06/17.
//

#include "MidiPlayback.h"

MidiPlayback::MidiPlayback() {
    _midi = new MidiOut();
    _osc.setup("127.0.0.1", 9898);
}

void MidiPlayback::setMidi(uint8_t port) {
    _midi->openPort(port);
}

void MidiPlayback::addNote(NoteEvent inputNote) {
    lock();
    bool exists = false;
    for (NoteEvent & note : _notes) {
        if (note.isNoteOff() && note.getTime() <= inputNote.getEndTime() &&
                note.getMidiPitch() == inputNote.getMidiPitch()) {
            exists = true;
        }
    }
    unlock();
    if (exists) return;
    lock();
    ofxOscMessage msg;
    lock();
    msg.setAddress("/note/" + inputNote.getTriggerName() + "/" +
            ofToString((uint32_t)inputNote.getChannel()) + (inputNote.getTriggerValue() <= 0 ? "/down" : ""));
    msg.addIntArg(inputNote.getMidiPitch());
    msg.addIntArg(inputNote.getMidiVelocity());
    msg.addIntArg(1);
    _osc.sendMessage(msg, true);
    _notes.push_back(inputNote);
    NoteEvent offNote = NoteEvent(inputNote.getEndTime(), 0,
            inputNote.getPitch(), inputNote.getVelocity(), inputNote.getChannel(), inputNote.getTriggerName(), inputNote.getTriggerValue());
    offNote.setNoteOff(true);
    _notes.push_back(offNote);
    unlock();
}

bool sortNotesByTime(NoteEvent &a, NoteEvent &b) {
    return a.getTime() < b.getTime();
}

void MidiPlayback::threadedFunction() {
    while (isThreadRunning()) {
        uint64_t i = 0, noteCount = _notes.size();
        ofxOscBundle bundle;
        lock();
        std::sort(_notes.begin(), _notes.end(), sortNotesByTime);
        unlock();

        while (noteCount > 0 && i < noteCount) {
            ofxOscMessage msg;
            lock();
            msg.setAddress("/note/" + _notes[i].getTriggerName() + "/" +
                    ofToString((uint32_t)_notes[i].getChannel()) + (_notes[i].getTriggerValue() <= 0 ? "/down" : ""));
            msg.addIntArg(_notes[i].getMidiPitch());
            msg.addIntArg(_notes[i].getMidiVelocity());
            if (_notes[i].getTime() <= _time.getTimeMillis()) {
                if (_notes[i].isNoteOff()) {
                    msg.addIntArg(0);
                    _midi->noteOff(_notes[i].getChannel(), _notes[i]);
                } else {
                    msg.addIntArg(1);
                    _midi->noteOn(_notes[i].getChannel(), _notes[i]);
                }
                _notes.erase(_notes.begin() + i, _notes.begin() + i + 1);
                noteCount = _notes.size();
            } else {
                i++;
            }
            unlock();
            bundle.addMessage(msg);
        }

        _osc.sendBundle(bundle);
        /*
        milliseconds slt(5);
        std::this_thread::sleep_for(slt);
         */
    }
    ofLogNotice() << "MidiPlayback exited" << endl;
}

vector<string> MidiPlayback::getPorts() {
    return _midi->getPorts();
}