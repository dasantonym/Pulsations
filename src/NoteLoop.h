//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_NOTELOOP_H
#define PULSATIONS_NOTELOOP_H

#include "ofMain.h"
#include "TimeKeeper.h"
#include "MidiOut.h"

class NoteLoop {
public:
    NoteLoop();

    void update();

    void setMidiOut(MidiOut *midiOut);
    void setRecord(bool record);
    void setMute(bool mute);

    void addNote(NoteEvent note);
    void addNote(uint64_t duration, float pitch, float velocity, uint8_t channel, string triggerName, float triggerValue);

    bool isMuted();
    bool isRecording();

    uint64_t getDuration();

private:
    MidiOut *_midiOut;
    TimeKeeper time;

    vector<NoteEvent> _notes;

    uint64_t _recording_start;
    uint64_t _playback_start;
    uint64_t _duration;
    uint32_t _position;

    bool _recording;
    bool _muted;
};


#endif //PULSATIONS_NOTELOOP_H
