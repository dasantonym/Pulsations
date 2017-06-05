//
// Created by anton on 05/06/17.
//

#include "MidiPlayback.h"

MidiPlayback::MidiPlayback() {
    _midi = new MidiOut();
}

void MidiPlayback::setMidi(uint8_t port) {
    _midi->openPort(port);
}

void MidiPlayback::addNote(NoteEvent inputNote) {
    lock();
    _notes.push_back(inputNote);
    NoteEvent offNote = NoteEvent(inputNote.getEndTime(), 0,
            inputNote.getPitch(), inputNote.getVelocity(), inputNote.getChannel());
    offNote.setNoteOff(true);
    _notes.push_back(offNote);
    unlock();
}

bool sortNotesByTime(NoteEvent &a, NoteEvent &b) {
    return a.getTime() < b.getTime();
}

void MidiPlayback::threadedFunction() {
    while (isThreadRunning()) {
        lock();
        uint64_t i = 0, noteCount = _notes.size();
        std::sort(_notes.begin(), _notes.end(), sortNotesByTime);
        unlock();

        while (noteCount > 0 && i < noteCount) {
            lock();
            if (_notes[i].getTime() <= _time.getTimeMillis()) {
                if (_notes[i].isNoteOff()) {
                    _midi->noteOff(_notes[i].getChannel(), _notes[i]);
                } else {
                    _midi->noteOn(_notes[i].getChannel(), _notes[i]);
                }
                _notes.erase(_notes.begin() + i, _notes.begin() + i + 1);
                noteCount = _notes.size();
            } else {
                i++;
            }
            unlock();
        }

        milliseconds slt(5);
        std::this_thread::sleep_for(slt);
    }
    ofLogNotice() << "MidiPlayback exited" << endl;
}

vector<string> MidiPlayback::getPorts() {
    return _midi->getPorts();
}