#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    _scaling = { 1.f, 1.f };

    sprintf(_version, "v%u.%u.%u", Version::VERSION_MAJ, Version::VERSION_MIN, Version::VERSION_PATCH);

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

    _drawGraph = (bool) settings.getValue("ui:drawGraph", true);
    _drawGui = (bool) settings.getValue("ui:drawGui", false);

    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gui->setAutoDraw(false, 0);
    gui->addHeader(":: PULSATIONS " + ofToString(_version) + " ::");

    gui->addFRM();

    gui->addToggle("Draw graph", _drawGraph);

    ofxDatGuiFolder *xbee = gui->addFolder("XBee", ofColor::blue);
    xbee->addTextInput("Device name", ofToString(settings.getValue("xbee:serial:deviceName", "cu.usbserial-AH01H39A")));
    xbee->addTextInput("Baud", ofToString(settings.getValue("xbee:serial:baud", 57600)));

    ofxDatGuiFolder *osc = gui->addFolder("OSC", ofColor::white);
    osc->addTextInput("Input port", ofToString(settings.getValue("osc:inputPort", 8888)));
    osc->addTextInput("Forward IP", settings.getValue("osc:forwardIP", "127.0.0.1"));
    osc->addTextInput("Forward port", ofToString(settings.getValue("osc:forwardPort", 9999)));

    oscSerialA = new OscSerial(settings.getValue("osc:forwardIP", "127.0.0.1"), settings.getValue("osc:forwardPort", 9999));
    if (oscSerialA->setup(
            settings.getValue("xbee:serial:a:deviceName", "cu.usbserial-AH01H39A"),
            (uint32_t)settings.getValue("xbee:serial:a:baud", 57600)
    )) {
        oscSerialA->startThread(true);
    }

    oscSerialB = new OscSerial(settings.getValue("osc:forwardIP", "127.0.0.1"), settings.getValue("osc:forwardPort", 9999));
    if (oscSerialB->setup(
            settings.getValue("xbee:serial:b:deviceName", "cu.usbserial-DN02N1QK"),
            (uint32_t)settings.getValue("xbee:serial:b:baud", 57600)
    )) {
        oscSerialB->startThread(true);
    }

    midiPlayback = new MidiPlayback(settings.getValue("osc:forwardIP", "127.0.0.1"), settings.getValue("osc:forwardPort", 9999));
    midiPlayback->setMidi((uint8_t)settings.getValue("midi:outputPort", 0));

    noteGenerator = new NoteGenerator();

    ofxDatGuiFolder *midi = gui->addFolder("MIDI", ofColor::blue);
    for (int i = 0; i < midiPlayback->getPorts().size(); i++) {
        midi->addToggle(midiPlayback->getPorts()[i], i == 0);
    }

    for (int i = 0; i < layout.get().getValue("layout:sensorcount", 3); i++) {
        string valuePath = "layout:sensor" + ofToString(i+1);

        Sensor *sensor = new Sensor(layout.get().getValue(valuePath + ":sid", ofToString(i+1)),
                layout.get().getValue(valuePath + ":name", "BNO055 IMU Fusion Sensor"),
                layout.get().getValue(valuePath + ":type", "u"), i);

        ofxDatGuiFolder *sensorUI = gui->addFolder("BNO055 Sensor #" + ofToString(i + 1), ofColor::yellow);
        sensorUI->addTextInput("Sensor ID #" + ofToString(i+1),
                layout.get().getValue(valuePath + ":sid", ofToString(i+1)));
        sensorUI->addSlider("Buffer (ms) #" + ofToString(i+1), 10, 10000,
                layout.get().getValue(valuePath + ":buffer", 1000));

        for (int t = 0; t < layout.get().getValue(valuePath + ":triggercount", 1); t++) {
            string tid = valuePath + ":trigger" + ofToString(t+1);
            string target = layout.get().getValue(tid + ":target", "acceleration");
            string name = layout.get().getValue(tid + ":name", "abs" + ofToString(i+1));
            bool absolute = (layout.get().getValue(tid + ":absolute", 1) == 1);
            bool range = (layout.get().getValue(tid + ":range", 0) == 1);
            int debounce = layout.get().getValue(tid + ":debounce", 500);
            
            ofVec3f mask = ofVec3f((float)layout.get().getValue(tid + ":mask:x", 0.f),
                    (float)layout.get().getValue(tid + ":mask:y", 0.f),
                    (float)layout.get().getValue(tid + ":mask:z", 0.f));
            
            ofVec3f falloff = ofVec3f((float)layout.get().getValue(tid + ":falloff:x", 0.f),
                    (float)layout.get().getValue(tid + ":falloff:y", 0.f),
                    (float)layout.get().getValue(tid + ":falloff:z", 0.f));

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

            Trigger3D* trigger;
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

                trigger = sensor->addTrigger(name, target, valueLow, valueHigh, absolute);
            } else {
                trigger = sensor->addTrigger(name, target, valueLow, absolute);
            }

            trigger->setDebounce(debounce);
            trigger->setMask(mask);
            trigger->setFalloff(falloff);
        }

        sensor->setGraph(ofPoint(80.f, 80.f + 160.f * i), ofGetWindowWidth() - 80.f, 140.f);

        sensors.push_back(sensor);
    }

    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);

    midiPlayback->startThread(true);
}

//--------------------------------------------------------------
void ofApp::update(){
    uint8_t idx = 0;
    for (Sensor * sensor : sensors) {
        if (oscSerialA->hasFrames(idx)) {
            sensor_frame_t frame = oscSerialA->getMaxFrame(idx);
            sensors[idx]->addFrame(frame);
            sensor_status_t status = oscSerialA->getStatus(idx);
            sensor->setCalibrationStatus(status.calibration);
        }

        if (oscSerialB->hasFrames(idx)) {
            sensor_frame_t frame = oscSerialB->getMaxFrame(idx);
            sensors[idx]->addFrame(frame);
            sensor_status_t status = oscSerialB->getStatus(idx);
            sensor->setCalibrationStatus(status.calibration);
        }

        sensor->update();
		/*
        for (Trigger3D * trigger : sensor->getTriggers()) {
            vector<NoteEvent> notes = noteGenerator->evaluateTriggerResult(trigger->getTriggerResult());
            for (NoteEvent & noteEvent : notes) {
                midiPlayback->addNote(noteEvent);
            }
            trigger->update();
        }
		*/
        idx++;
    }

    if (_drawGui) {
        gui->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofScale(_scaling);
    if (_drawGraph) {
        for (Sensor * sensor : sensors) {
            sensor->draw();
        }
    }
    if (_drawGui) {
        gui->draw();
    }
    ofPushStyle();
    ofSetColor(120, 120, 120);
    ofDrawBitmapString(_version, ofGetWindowWidth() - 110.f, ofGetWindowHeight() - 60.f);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
        case ' ':
            _drawGui = !_drawGui;
            settings.setValue("ui:drawGui", _drawGui);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    _scaling.x = w / 1024.f;
    _scaling.y = h / 768.f;
}

//--------------------------------------------------------------
void ofApp::exit() {
    settings.save("settings.xml");
    layout.save();
}

//--------------------------------------------------------------
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {
    if (e.target->getLabel() == "Draw graph") {
        _drawGraph = !_drawGraph;
        settings.setValue("ui:drawGraph", _drawGraph);
    }
}

//--------------------------------------------------------------
void ofApp::onSliderEvent(ofxDatGuiSliderEvent e) {
    // TODO: fix the gui
    sensor_trigger_t trigger;
    int8_t id = -1;

    if (e.target->getLabel() == "Buffer (ms) #1") {
        id = 0;
    } else if (e.target->getLabel() == "Buffer (ms) #2") {
        id = 1;
    } else if (e.target->getLabel() == "Buffer (ms) #3") {
        id = 2;
    }
    if (id >= 0) {
        //sensors[id].setBufferSizeMillis((uint64_t) e.target->getValue());
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
