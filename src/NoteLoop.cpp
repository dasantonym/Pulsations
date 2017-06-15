//
// Created by anton on 04/06/17.
//

#include "NoteLoop.h"


//
//
// Constructors
//

NoteLoop::NoteLoop() {
    _recording = false;
    _muted = true;
    _duration = 0;
    _position = 0;
}


//
//
// Main
//

void NoteLoop::update() {
    if (_recording || _muted || _notes.size() == 0) {
        return;
    }
    if (_position >= _notes.size() * 2) {
        _playback_start = time.getTimeMillis();
        _position = 0;
    }
    uint32_t index = (uint32_t) floorf((float)_position * .5f);
    if (_position % 2 == 0) {
        if (_notes[index].getTime() + _playback_start <= time.getTimeMillis()) {
            _midiOut->noteOn(_notes[index].getChannel(), _notes[index]);
            _position++;
        }
    } else {
        if (_notes[index].getEndTime() + _playback_start <= time.getTimeMillis()) {
            _midiOut->noteOff(_notes[index].getChannel(), _notes[index]);
            _position++;
        }
    }
}

void NoteLoop::addNote(NoteEvent note) {
    if (_recording) {
        NoteEvent noteAdd = NoteEvent(note.getTime(), note.getEndTime() - note.getTime(), note.getPitch(), note.getVelocity(), note.getChannel(), note.getTriggerName(), note.getTriggerValue());
        _notes.push_back(noteAdd);
    }
}

void NoteLoop::addNote(uint64_t duration, float pitch, float velocity, uint8_t channel, string triggerName, float triggerValue) {
    if (_recording) {
        NoteEvent note = NoteEvent(time.getTimeMillis() - _recording_start, duration, pitch, velocity, channel, triggerName, triggerValue);
        addNote(note);
    }
}


//
//
// Setters
//

void NoteLoop::setMidiOut(MidiOut *midiOut) {
    _midiOut = midiOut;
}

void NoteLoop::setMute(bool mute) {
    _muted = mute;
}

void NoteLoop::setRecord(bool record) {
    if (record) {
        _notes.clear();
        _position = 0;
        _muted = true;
        _recording = true;
        _recording_start = time.getTimeMillis();
    } else {
        _recording = false;
        _muted = false;
        _playback_start = time.getTimeMillis();
        _duration = _playback_start - _recording_start;
    }
}


//
//
// Getters
//

bool NoteLoop::isMuted() {
    return _muted;
}

bool NoteLoop::isRecording() {
    return _recording;
}

uint64_t NoteLoop::getDuration() {
    return _duration;
}
