#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);

    settings.load("settings.xml");

    if (settings.getValue("layout", "empty") != "empty") {
        layout.load(settings.getValue("layout", "empty"));
    }
    if (layout.get().getValue("layout:sensorcount", 0) == 0) {
        string layoutName = "empty-" + ofGetTimestampString();
        layout.init(layoutName);
        layout.save();
        settings.setValue("layout", layoutName);
    }

    _isRecordingLoop = false;
    _drawCurves = (bool) settings.getValue("ui:drawCurves", true);

    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gui->addHeader(":: PULSATIONS ::");

    gui->addFRM();

    gui->addToggle("Draw curves", _drawCurves);

    ofxDatGuiFolder *osc = gui->addFolder("OSC", ofColor::white);
    osc->addTextInput("Input port", ofToString(settings.getValue("osc:inputPort", 8888)));
    osc->addTextInput("Forward IP", settings.getValue("osc:forwardIP", "127.0.0.1"));
    osc->addTextInput("Forward port", ofToString(settings.getValue("osc:forwardPort", 9999)));

    for (int i = 0; i < layout.get().getValue("layout:sensorcount", 3); ++i) {
        string valuePath = "sensor:sn" + ofToString(i+1);

        ofxDatGuiFolder *sensorUI = gui->addFolder("BNO055 Sensor #" + ofToString(i + 1), ofColor::yellow);
        sensorUI->addTextInput("Sensor ID #" + ofToString(i + 1), layout.get().getValue(valuePath + ":sid", "10" + ofToString(i)));
        Sensor sensor = Sensor(
                layout.get().getValue(valuePath + ":sid", "10" + ofToString(i)), "BNO 055 IMU Fusion Sensor", "bno055");

        for (int t = 0; t < layout.get().getValue(valuePath + ":triggercount", 1); ++t) {
            string tid = valuePath + ":trigger:tr" + ofToString(t+1);
            string target = layout.get().getValue(tid + ":target", "acceleration");
            string name = layout.get().getValue(tid + ":name", "abs" + ofToString(i+1));
            bool absolute = layout.get().getValue(tid + ":absolute", 1) == 1;
            bool range = layout.get().getValue(tid + ":range", 0) == 1;
            int debounce = layout.get().getValue(tid + ":debounceMillis", 1000);

            ofVec3f valueLow, valueHigh;;
            valueLow.x = (float) layout.get().getValue(tid + ":low:x", 6.8f);
            valueLow.y = (float) layout.get().getValue(tid + ":low:y", 6.8f);
            valueLow.z = (float) layout.get().getValue(tid + ":low:z", 6.8f);

            if (range) {
                valueHigh.x = (float) layout.get().getValue(tid + ":high:x", 6.8f);
                valueHigh.y = (float) layout.get().getValue(tid + ":high:y", 6.8f);
                valueHigh.z = (float) layout.get().getValue(tid + ":high:z", 6.8f);
            }

            sensor.addTrigger(name, target, valueLow, valueHigh, absolute);
            sensor.getTriggers()[sensor.getTriggers().size() - 1].trigger->setDebounce(debounce);
        }

        sensor.setGraph(ofPoint(40.f, 40.f + 120.f * i), ofGetWindowWidth() - 80.f, 100.f);
        sensors.push_back(sensor);
    }

    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);

    midiOut = new MidiOut();
    midiOut->openPort(0);

    sender.setup(settings.getValue("osc:forwardIP", "127.0.0.1"), settings.getValue("osc:forwardPort", 9999));
    receiver.setup(settings.getValue("osc:inputPort", 8888));
}

//--------------------------------------------------------------
void ofApp::update(){
    ofxOscBundle bundle;
    uint8_t count = 0;
    uint64_t frame_time = ofGetElapsedTimeMillis();

    while (receiver.hasWaitingMessages()) {
        ofxOscMessage msg;
        receiver.getNextMessage(msg);
        vector<string> address = ofSplitString(msg.getAddress(), "/", true, true);
        if (address.size() == 2 && address[0] == "bno055") {
            for (Sensor & sensor : sensors) {
                if (!sensor.getStatus().active) {
                    continue;
                }
                if (sensor.hasOSCAddress(msg.getAddress())) {
                    uint64_t time = frame_time;
                    ofVec3f acceleration, orientation;
                    for (int i = 1; i < msg.getNumArgs(); ++i) {
                        string type = msg.getArgTypeName(i);
                        if (type == "f") {
                            switch (i) {
                                case 1:
                                    orientation.x = msg.getArgAsFloat(i);
                                    break;
                                case 2:
                                    orientation.y = msg.getArgAsFloat(i);
                                    break;
                                case 3:
                                    orientation.z = msg.getArgAsFloat(i);
                                    break;
                                case 4:
                                    acceleration.x = msg.getArgAsFloat(i);
                                    break;
                                case 5:
                                    acceleration.y = msg.getArgAsFloat(i);
                                    break;
                                case 6:
                                    acceleration.z = msg.getArgAsFloat(i);
                                    break;
                                default:
                                    break;
                            }
                        } else if (type == "b") {
                            sensor.getStatus().calibration = msg.getArgAsBlob(i);
                        } else if (type == "t") {
                            time = frame_time;
                        }
                    }
                    sensor.addFrame(time, frame_time, acceleration, orientation);
                }
            }
        }
    }

    count = 0;
    for (Sensor & sensor : sensors) {
        if (!sensor.getStatus().active) {
            continue;
        }

        sensor.update();

        uint8_t tcount = 0;
        string sensorPath = "sensor" + ofToString(count + 1);

        for (sensor_trigger_3d_t & trigger : sensor.getTriggers()) {
            if (trigger.trigger->isTriggered()) {
                string triggerPath = sensorPath + ":trigger" + ofToString(tcount + 1);
                ofxOscMessage msgOut;
                ofVec3f val = trigger.trigger->getTrigger();
                msgOut.setAddress(sensor.getOSCAddress() + "/" + trigger.target + "/" + trigger.name);
                msgOut.addFloatArg(val.x);
                msgOut.addFloatArg(val.y);
                msgOut.addFloatArg(val.z);
                bundle.addMessage(msgOut);

                if (val.x > 0.f) {
                    NoteEvent noteX = NoteEvent(
                            frame_time,
                            (uint64_t)layout.get().getValue(triggerPath + ":midi:x:duration", 250),
                            layout.get().getValue(triggerPath + ":midi:x:pitch", 64),
                            layout.get().getValue(triggerPath + ":midi:x:velocity", 128)
                    );
                    midiOut->noteOn((uint8_t)layout.get().getValue(triggerPath + ":midi:x:channel", 1), noteX);
                    _openNotes.push_back(noteX);
                    if (_isRecordingLoop) {
                        _loops[_loops.size() - 1].addNote(noteX);
                    }
                }

                if (val.y > 0.f) {
                    NoteEvent noteY = NoteEvent(
                            frame_time,
                            (uint64_t)layout.get().getValue(triggerPath + ":midi:y:duration", 250),
                            layout.get().getValue(triggerPath + ":midi:y:pitch", 64),
                            layout.get().getValue(triggerPath + ":midi:y:velocity", 128)
                    );
                    midiOut->noteOn((uint8_t)layout.get().getValue(triggerPath + ":midi:y:channel", 1), noteY);
                    _openNotes.push_back(noteY);
                    if (_isRecordingLoop) {
                        _loops[_loops.size() - 1].addNote(noteY);
                    }
                }

                if (val.z > 0.f) {
                    NoteEvent noteZ = NoteEvent(
                            frame_time,
                            (uint64_t)layout.get().getValue(triggerPath + ":midi:z:duration", 250),
                            layout.get().getValue(triggerPath + ":midi:z:pitch", 64),
                            layout.get().getValue(triggerPath + ":midi:z:velocity", 128)
                    );
                    midiOut->noteOn((uint8_t)layout.get().getValue(triggerPath + ":midi:z:channel", 1), noteZ);
                    _openNotes.push_back(noteZ);
                    if (_isRecordingLoop) {
                        _loops[_loops.size() - 1].addNote(noteZ);
                    }
                }

            }
        }
    }

    count = 0;
    while (_openNotes.size() > 0 && count < _openNotes.size()) {
        NoteEvent & note = _openNotes[0];
        if (note.getEndTime() <= ofGetElapsedTimeMillis()) {
            midiOut->noteOff(count + 1, note);
            _openNotes.erase(_openNotes.begin() + count, _openNotes.begin() + 1);
        } else {
            count++;
        }
    }

    for (NoteLoop & loop : _loops) {
        if (loop.isMuted() || loop.getDuration() == 0) {
            continue;
        }

        loop.update();
    }

    if (bundle.getMessageCount() > 0) {
        sender.sendBundle(bundle);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    uint8_t count = 0;
    for (Sensor & sensor : sensors) {
        if (!sensor.getStatus().active) {
            continue;
        }

        if (_drawCurves) {
            sensor.draw();
        }

        ofDrawBitmapString(sensor.getOSCAddress(), 40.f, 40.f + 120.f * count);
        ofDrawBitmapString(sensor.getCalibrationStatus(), 160.f, 40.f + 120.f * count);

        if (sensor.hasFrames()) {
            ofDrawBitmapString(ofToString(sensor.getCurrentFrame().acceleration), 220.f, 40.f + 120.f * count);
            ofDrawBitmapString(ofToString(sensor.getCurrentFrame().orientation), 380.f, 40.f + 120.f * count);

            uint32_t tc = 0;
            for (sensor_trigger_3d_t & trigger : sensor.getTriggers()) {
                ofDrawBitmapString(ofToString(trigger.trigger->getTrigger()), 40.f + 160.f * tc, 60.f + 120.f * count);
                tc++;
            }
        }

        count++;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
        case ' ':
            _isRecordingLoop = !_isRecordingLoop;
            if (_isRecordingLoop) {
                NoteLoop loop = NoteLoop();
                loop.setMidiOut(midiOut);
                loop.setRecord(true);
                _loops.push_back(loop);
            } else {
                _loops[_loops.size() - 1].setRecord(false);
            }
            break;
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::exit() {
    settings.save("settings.xml");
    layout.save();
}

//--------------------------------------------------------------
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {
    if (e.target->getLabel() == "Draw curves") {
        _drawCurves = !_drawCurves;
        settings.setValue("ui:drawCurves", _drawCurves);
    }
}

//--------------------------------------------------------------
void ofApp::onSliderEvent(ofxDatGuiSliderEvent e) {
    sensor_trigger_3d_t trigger;
    int8_t id = -1;
    if (e.target->getLabel() == "Trigger ACC #1") {
        id = 0;
    } else if (e.target->getLabel() == "Trigger ACC #2") {
        id = 1;
    } else if (e.target->getLabel() == "Trigger ACC #3") {
        id = 2;
    }
    if (id >= 0) {
        sensors[id].getTriggers()[sensors[id]
                .getTriggerIndexByName("acc" + ofToString(id + 1))].trigger->setTrigger((float) e.target->getValue());
        // settings.setValue("sensor:id" + ofToString(id + 1) + ":accelerationThreshold", e.target->getValue());
    }
}

//--------------------------------------------------------------
void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e) {
    if (e.target->getLabel() == "Input port") {
        settings.setValue("osc:inputPort", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Forward IP") {
        settings.setValue("osc:forwardIP", e.target->getText());
    } else if (e.target->getLabel() == "Forward port") {
        settings.setValue("osc:forwardPort", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Sensor ID #1") {
        settings.setValue("sensor:sn1:sid", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Sensor ID #2") {
        settings.setValue("sensor:sn2:sid", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Sensor ID #3") {
        settings.setValue("sensor:sn3:sid", ofToInt(e.target->getText()));
    }
}