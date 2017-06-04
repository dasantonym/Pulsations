#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    
    frameCount = 0;

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
    _overdub = (bool) settings.getValue("record:overdub", true);

    string version = "v0.1.0";// + ofToString(Version::VERSION_MAJ) + "." + ofToString(Version::VERSION_MIN) +
            // "." + ofToString(Version::VERSION_PATCH);
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gui->addHeader(":: PULSATIONS " + version + " ::");

    gui->addFRM();

    gui->addToggle("Draw curves", _drawCurves);
    gui->addToggle("Overdub", _overdub);

    ofxDatGuiFolder *osc = gui->addFolder("OSC", ofColor::white);
    osc->addTextInput("Input port", ofToString(settings.getValue("osc:inputPort", 8888)));
    osc->addTextInput("Forward IP", settings.getValue("osc:forwardIP", "127.0.0.1"));
    osc->addTextInput("Forward port", ofToString(settings.getValue("osc:forwardPort", 9999)));
    osc->addTextInput("Remote resp. IP", ofToString(settings.getValue("osc:responseIP", "192.168.0.255")));
    osc->addTextInput("Remote resp. port", ofToString(settings.getValue("osc:responsePort", 7777)));

    for (int i = 0; i < layout.get().getValue("layout:sensorcount", 3); ++i) {
        string valuePath = "layout:sensor" + ofToString(i+1);

        ofxDatGuiFolder *sensorUI = gui->addFolder("BNO055 Sensor #" + ofToString(i + 1), ofColor::yellow);
        sensorUI->addTextInput("Sensor ID #" + ofToString(i+1),
                layout.get().getValue(valuePath + ":sid", "10" + ofToString(i)));
        sensorUI->addSlider("Buffer (ms) #" + ofToString(i+1), 10, 10000,
                layout.get().getValue(valuePath + ":buffer", 1000));

        Sensor sensor = Sensor(
                layout.get().getValue(valuePath + ":sid", "10" + ofToString(i)),
                "BNO 055 IMU Fusion Sensor", "bno055");

        for (int t = 0; t < layout.get().getValue(valuePath + ":triggercount", 1); ++t) {
            string tid = valuePath + ":trigger" + ofToString(t+1);
            string target = layout.get().getValue(tid + ":target", "acceleration");
            string name = layout.get().getValue(tid + ":name", "abs" + ofToString(i+1));
            bool absolute = layout.get().getValue(tid + ":absolute", 1) == 1;
            bool range = layout.get().getValue(tid + ":range", 0) == 1;
            int debounce = layout.get().getValue(tid + ":debounce", 500);
            
            ofVec3f mask = ofVec3f(layout.get().getValue(tid + ":mask:x", 0.f),
                                   layout.get().getValue(tid + ":mask:y", 0.f),
                                   layout.get().getValue(tid + ":mask:z", 0.f));
            
            ofVec3f falloff = ofVec3f(layout.get().getValue(tid + ":falloff:x", 0.f),
                                      layout.get().getValue(tid + ":falloff:y", 0.f),
                                      layout.get().getValue(tid + ":falloff:z", 0.f));

            ofxDatGuiFolder *triggerUI = gui->addFolder("Sensor #" + ofToString(i + 1) +
                    " Trigger #" + ofToString(t+1), ofColor::yellow);

            triggerUI->addToggle("Absolute", absolute);
            triggerUI->addSlider("Debounce (ms)", 0, 4000, layout.get().getValue(tid + ":debounce", 500));

            triggerUI->addSlider("Falloff X", 0.f, 1.f, falloff.x);
            triggerUI->addSlider("Falloff Y", 0.f, 1.f, falloff.y);
            triggerUI->addSlider("Falloff Z", 0.f, 1.f, falloff.z);
            
            triggerUI->addSlider("Mask X", 0.f, 1.f, mask.x);
            triggerUI->addSlider("Mask Y", 0.f, 1.f, mask.y);
            triggerUI->addSlider("Mask Z", 0.f, 1.f, mask.z);

            float rangeMin = target == "acceleration" ? -20.f : -180.f;
            float rangeMax = target == "acceleration" ? 20.f : 180.f;

            ofVec3f valueLow, valueHigh;;
            valueLow.x = (float) layout.get().getValue(tid + ":low:x", 6.8f);
            triggerUI->addSlider("Low X",
                    target == "orientation" ? 0.f : rangeMin,
                    target == "orientation" ? 360.f : rangeMax,
                    valueLow.x);

            valueLow.y = (float) layout.get().getValue(tid + ":low:y", 6.8f);
            triggerUI->addSlider("Low Y", rangeMin, rangeMax, valueLow.y);

            valueLow.z = (float) layout.get().getValue(tid + ":low:z", 6.8f);
            triggerUI->addSlider("Low Z", rangeMin, rangeMax, valueLow.z);

            if (range) {
                valueHigh.x = (float) layout.get().getValue(tid + ":high:x", 6.8f);
                triggerUI->addSlider("High X",
                        target == "orientation" ? 0.f : rangeMin,
                        target == "orientation" ? 360.f : rangeMax,
                        valueHigh.x);

                valueHigh.y = (float) layout.get().getValue(tid + ":high:y", 6.8f);
                triggerUI->addSlider("High Y", rangeMin, rangeMax, valueHigh.y);

                valueHigh.z = (float) layout.get().getValue(tid + ":high:z", 6.8f);
                triggerUI->addSlider("High Z", rangeMin, rangeMax, valueHigh.z);
                
                sensor.addTrigger(name, target, valueLow, valueHigh, absolute);
            } else {
                sensor.addTrigger(name, target, valueLow, absolute);
            }
            
            sensor.getTriggers()[sensor.getTriggers().size() - 1].trigger->setDebounce(debounce);
            sensor.getTriggers()[sensor.getTriggers().size() - 1].trigger->setMask(mask);
            sensor.getTriggers()[sensor.getTriggers().size() - 1].trigger->setFalloff(falloff);
        }

        sensor.setGraph(ofPoint(240.f + 20.f, 40.f + 90.f * i), ofGetWindowWidth() - 40.f, 100.f);

        sensors.push_back(sensor);
    }

    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);

    midiOut = new MidiOut();
    midiOut->openPort(0);

    uiResponder.setup(settings.getValue("osc:responseIP", "192.168.0.255"),
                      settings.getValue("osc:responsePort", 7777));
    receiver.setup(settings.getValue("osc:inputPort", 8888));
    sender.setup(settings.getValue("osc:forwardIP", "127.0.0.1"), settings.getValue("osc:forwardPort", 9999));
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

        remote_command_t response = _remoteControl.parseCommand(msg);
        if (response.command != RemoteControl::COM_UNKNOWN) {
            ofxOscMessage res;
            res.setAddress(msg.getAddress());
            switch (response.command) {
                case RemoteControl::COM_TOGGLE_RECORD_LOOP:
                    toggleLoop((msg.getArgAsInt(0) == 1));
                    res.addBoolArg(_isRecordingLoop);
                    uiResponder.sendMessage(res, true);
                    break;
                case RemoteControl::COM_TOGGLE_OVERDUB:
                    _overdub = (msg.getArgAsInt(0) == 1);
                    res.addBoolArg(_overdub);
                    uiResponder.sendMessage(res, true);
                    break;
                default:
                    break;
            }
        }
    }
    
    if (frameCount >= 200) {
        ofxOscMessage res;
        res.setAddress("/com/0");
        res.addIntArg(_isRecordingLoop);
        uiResponder.sendMessage(res, true);
        res.clear();
        res.setAddress("/com/1");
        res.addIntArg(_overdub);
        uiResponder.sendMessage(res, true);
        
        frameCount = 0;
    }

    count = 0;
    for (Sensor & sensor : sensors) {
        if (!sensor.getStatus().active) {
            continue;
        }

        sensor.update();

        uint8_t tcount = 0;
        string sensorPath = "layout:sensor" + ofToString(count + 1);

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
                            layout.get().getValue(triggerPath + ":midi:x:pitch", .5f),
                            layout.get().getValue(triggerPath + ":midi:x:velocity", .8f)
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
                            layout.get().getValue(triggerPath + ":midi:y:pitch", .5f),
                            layout.get().getValue(triggerPath + ":midi:y:velocity", .8f)
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
                            layout.get().getValue(triggerPath + ":midi:z:pitch", .5f),
                            layout.get().getValue(triggerPath + ":midi:z:velocity", .8f)
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
            midiOut->noteOff((uint8_t)(count + 1), note);
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
    
    frameCount += 1;
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

        if (sensor.hasFrames()) {
            ofDrawBitmapString(sensor.getDataAsString(), 40.f, 40.f + 90.f * count);

            uint32_t tc = 0;
            for (sensor_trigger_3d_t & trigger : sensor.getTriggers()) {
                ofDrawBitmapString(trigger.name + " " + trigger.trigger->getTriggerAsString() + "   ",
                        40.f + 160.f * tc, 40.f + 90.f * (count+1) - 70.f);
                tc++;
            }
            
            ofPoint x1 = ofPoint(-20.f, 25.f);
            ofPoint x2 = ofPoint(20.f, 25.f);
            ofPoint y1 = ofPoint(.0f, -25.f);

            ofPushStyle();
            ofFill();
            ofSetColor(180, 0, 0);

            ofPushMatrix();
            ofTranslate(40.f + 35.f, 40.f + 100.f * (count+1) - 40.f);
            ofRotate(sensor.getCurrentFrame().orientation.x);
            ofDrawTriangle(x1, x2, y1);
            ofPopMatrix();

            ofSetColor(180, 180, 0);
            ofPushMatrix();
            ofTranslate(40.f + 35.f + 70.f, 40.f + 100.f * (count+1) - 40.f);
            ofRotate(sensor.getCurrentFrame().orientation.y);
            ofDrawTriangle(x1, x2, y1);
            ofPopMatrix();

            ofSetColor(0, 180, 180);
            ofPushMatrix();
            ofTranslate(40.f + 35.f + 70.f * 2.f, 40.f + 100.f * (count+1) - 40.f);
            ofRotate((sensor.getCurrentFrame().orientation.z * -1.f) + 90.f);
            ofDrawTriangle(x1, x2, y1);
            ofPopMatrix();

            ofPopStyle();
        }

        count++;
    }

    if (_isRecordingLoop && _loops.size() == 0) {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("REC LOOP", 40.f, ofGetWindowHeight() - 60.f);
        ofPopStyle();
    } else if (_loops.size() > 0 && !_isRecordingLoop) {
        ofPushStyle();
        ofSetColor(0, 255, 0);
        ofDrawBitmapString("PLAY LOOP", 40.f, ofGetWindowHeight() - 60.f);
        ofPopStyle();
    } else if (_loops.size() > 0 && _isRecordingLoop) {
        ofPushStyle();
        ofSetColor(255, 180, 0);
        ofDrawBitmapString("OVERDUB LOOP", 40.f, ofGetWindowHeight() - 60.f);
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofApp::toggleLoop(bool loop){
    _isRecordingLoop = loop;
    if (_isRecordingLoop) {
        NoteLoop loop = NoteLoop();
        loop.setMidiOut(midiOut);
        loop.setRecord(true);
        _loops.push_back(loop);
    } else {
        _loops[_loops.size() - 1].setRecord(false);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
        case ' ':
            toggleLoop(!_isRecordingLoop);
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
    } else if (e.target->getLabel() == "Overdub") {
        _overdub = !_overdub;
        settings.setValue("record:overdub", _overdub);
    }
}

//--------------------------------------------------------------
void ofApp::onSliderEvent(ofxDatGuiSliderEvent e) {
    sensor_trigger_3d_t trigger;
    int8_t id = -1;

    if (e.target->getLabel() == "Buffer (ms) #1") {
        id = 0;
    } else if (e.target->getLabel() == "Buffer (ms) #2") {
        id = 1;
    } else if (e.target->getLabel() == "Buffer (ms) #3") {
        id = 2;
    }
    if (id >= 0) {
        sensors[id].setBufferSizeMillis((uint64_t) e.target->getValue());
        // settings.setValue("layout:sensor1" + ofToString(id + 1) + ":buffer", e.target->getValue());
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
        settings.setValue("layout:sensor1:sid", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Sensor ID #2") {
        settings.setValue("layout:sensor2:sid", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Sensor ID #3") {
        settings.setValue("layout:sensor3:sid", ofToInt(e.target->getText()));
    }
}
