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
    _positionController = 0;
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
        _playback_start = ofGetElapsedTimeMillis();
        _position = 0;
        _positionController = 0;
    }
    uint32_t index = (uint32_t) floorf((float)_position * .5f);
    if (_position % 2 == 0) {
        if (_notes[index].getTime() + _playback_start <= ofGetElapsedTimeMillis()) {
            _midiOut->noteOn(_notes[index].getChannel(), _notes[index]);
            _position++;
        }
    } else {
        if (_notes[index].getEndTime() + _playback_start <= ofGetElapsedTimeMillis()) {
            _midiOut->noteOff(_notes[index].getChannel(), _notes[index]);
            _position++;
        }
    }
    if (_positionController < _controllerEvents.size()) {
        _midiOut->sendController(_notes[index].getChannel(), _controllerEvents[_positionController]);
        _positionController++;
    }
}

void NoteLoop::addNote(NoteEvent note) {
    if (_recording) {
        NoteEvent noteAdd = NoteEvent(note.getTime(), note.getEndTime() - note.getTime(), note.getPitch(), note.getVelocity(), note.getChannel());
        _notes.push_back(noteAdd);
    }
}

void NoteLoop::addNote(uint64_t duration, float pitch, float velocity, uint8_t channel) {
    if (_recording) {
        NoteEvent note = NoteEvent(ofGetElapsedTimeMillis() - _recording_start, duration, pitch, velocity, channel);
        addNote(note);
    }
}

void NoteLoop::addControllerEvent(ControllerEvent event) {
    if (_recording) {
        _controllerEvents.push_back(event);
    }
}

void NoteLoop::addControllerEvent(uint8_t controller, float value) {
    if (_recording) {
        ControllerEvent event = ControllerEvent(ofGetElapsedTimeMillis() - _recording_start, controller, value);
        addControllerEvent(event);
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
        _recording_start = ofGetElapsedTimeMillis();
    } else {
        _recording = false;
        _muted = false;
        _playback_start = ofGetElapsedTimeMillis();
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
