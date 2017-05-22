#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);

    settings.load("settings.xml");

    isRecording = false;

    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gui->addHeader(":: PULSATIONS ::");

    gui->addFRM();

    gui->addFolder("OSC", ofColor::white);
    gui->addToggle("Record data", false);
    gui->addTextInput("Input port", ofToString(settings.getValue("osc:inputPort", 8888)));
    gui->addTextInput("Forward IP", settings.getValue("osc:forwardIP", "127.0.0.1"));
    gui->addTextInput("Forward port", ofToString(settings.getValue("osc:forwardPort", 9999)));

    gui->addFolder("BNO055 Sensor 1", ofColor::yellow);
    gui->addSlider("Trigger ACC 1", 0.f, 50.f, (float) settings.getValue("sensor:id1:accelerationThreshold", 8.f));

    gui->addFolder("BNO055 Sensor 2", ofColor::yellow);
    gui->addSlider("Trigger ACC 2", 0.f, 50.f, (float) settings.getValue("sensor:id2:accelerationThreshold", 8.f));

    gui->addFolder("BNO055 Sensor 3", ofColor::yellow);
    gui->addSlider("Trigger ACC 3", 0.f, 50.f, (float) settings.getValue("sensor:id3:accelerationThreshold", 8.f));

    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);

    sender.setup(settings.getValue("osc:forwardIP", "127.0.0.1"), settings.getValue("osc:forwardPort", 8888));
    receiver.setup(settings.getValue("osc:inputPort", 8888));

#ifdef USE_VIDEO
    videoGrabber.setDeviceID(0);
    videoGrabber.setDesiredFrameRate(25);
    videoGrabber.setup(1280, 720);

    videoRecorder.setup(tstamp + ".mp4", 1280, 720, 25);
    videoRecorder.start();
#endif

    for (int i = 0; i < 3; ++i) {
        sensor_source_t source;
        source.id = "10" + ofToString(i);
        source.type = "bno055";
        source.name = "BNO 055 IMU Fusion Sensor";
        source.startTime = 0;
        source.settings.active = true;
        source.settings.accelerationThreshold = (float) settings.getValue("sensor:id" + ofToString(i) + ":accelerationThreshold", 8.f);
        for (int i = 0; i < 6; ++i) {
            ofPath path;
            path.setFilled(false);
            path.setStrokeWidth(1.0f);
            ofColor color;
            switch (i) {
                case 0:
                    path.setStrokeColor(color.red);
                    break;
                case 1:
                    path.setStrokeColor(color.green);
                    break;
                case 2:
                    path.setStrokeColor(color.blue);
                    break;
                case 3:
                    path.setStrokeColor(color.yellow);
                    break;
                case 4:
                    path.setStrokeColor(color.cyan);
                    break;
                case 5:
                    path.setStrokeColor(color.magenta);
                    break;
            }
            source.paths.push_back(path);
        }
        sources.push_back(source);
    }
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {
    if (e.target->getLabel() == "Record data") {
        isRecording = e.target->getEnabled();
        if (isRecording) {
            recordingStart = ofGetSystemTime();
            recordingStartMicros = ofGetSystemTimeMicros();
            tstamp = ofGetTimestampString();
        } else {
#ifdef USE_VIDEO
            videoRecorder.close();
#endif
            for (sensor_source_t &source : sources) {
                if (source.frames.size() == 0) {
                    continue;
                }
                ofFile file;
                file.open(source.type + source.id + tstamp + ".txt", ofFile::WriteOnly);
                ofBuffer out;
                out.append(ofToString(recordingStart) + " " + ofToString(recordingStartMicros) + "\n");
                out.append(ofToString(source.startTime) + " " + ofToString(source.lastTime) + "\n");
                for (sensor_frame_t &frame : source.frames) {
                    out.append(ofToString(frame.time) + " ");
                    for (float &val : frame.data) {
                        out.append(ofToString(val) + " ");
                    }
                    string calibration = "";
                    for (char &c : frame.calibration) {
                        if (c == 0x1) {
                            calibration += "1";
                        } else if (c == 0x2) {
                            calibration += "2";
                        } else if (c == 0x3) {
                            calibration += "3";
                        } else {
                            calibration += "0";
                        }
                    }
                    out.append(calibration);
                    out.append("\n");
                }
                file.writeFromBuffer(out);

                out.clear();
                source.startTime = 0;
                source.lastTime = 0;
                source.frames.clear();
            }
        }
    }
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e) {
    if (e.target->getLabel() == "Trigger ACC 1") {
        sources[0].settings.accelerationThreshold = (float) e.target->getValue();
        settings.setValue("sensor:id1:accelerationThreshold", e.target->getValue());
    } else if (e.target->getLabel() == "Trigger ACC 2") {
        sources[1].settings.accelerationThreshold = (float) e.target->getValue();
        settings.setValue("sensor:id2:accelerationThreshold", e.target->getValue());
    } else if (e.target->getLabel() == "Trigger ACC 3") {
        sources[2].settings.accelerationThreshold = (float) e.target->getValue();
        settings.setValue("sensor:id3:accelerationThreshold", e.target->getValue());
    }
}

void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e) {
    if (e.target->getLabel() == "Input port") {
        settings.setValue("osc:inputPort", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Forward IP") {
        settings.setValue("osc:forwardIP", e.target->getText());
    } else if (e.target->getLabel() == "Forward port") {
        settings.setValue("osc:forwardPort", ofToInt(e.target->getText()));
    }
}

//--------------------------------------------------------------
void ofApp::update(){
#ifdef USE_VIDEO
    videoGrabber.update();
    if (videoGrabber.isFrameNew()) {
        videoRecorder.addFrame(videoGrabber.getPixels());
    }
#endif

    while (receiver.hasWaitingMessages()) {
        uint64_t time_received = ofGetSystemTime();
        ofxOscMessage msg;
        ofxOscMessage filteredMessage;
        receiver.getNextMessage(msg);
        vector<string> address = ofSplitString(msg.getAddress(), "/", true, true);
        if (address.size() == 2) {
            filteredMessage.setAddress(msg.getAddress());
            filteredMessage.addTimetagArg(0);
            for (sensor_source_t & source : sources) {
                if (source.type == address[0] && source.id == address[1]) {
                    // FIXME: timetags are not received properly...
                    sensor_frame_t frame;
                    for (int i = 0; i < msg.getNumArgs(); ++i) {
                        string type = msg.getArgTypeName(i);
                        if (type == "f") {
                            if (i > 3 && i < 8) {
                                if (fabs(msg.getArgAsFloat(i)) >= source.settings.accelerationThreshold) {
                                    filteredMessage.addFloatArg(msg.getArgAsFloat(i));
                                }
                            } else {
                                filteredMessage.addFloatArg(msg.getArgAsFloat(i));
                            }
                        } else if (type == "b") {
                            frame.calibration = msg.getArgAsBlob(i);
                        } else if (type == "T") {
                            if (source.startTime == 0) {
                                source.startTime = time_received;
                            }
                            source.lastTime = time_received;
                            frame.time = time_received;
                        }
                    }
                    if (frame.data.size() == 6) {
                        source.frames.push_back(frame);
                    }
                    if (!isRecording && source.frames.size() > 600) {
                        source.frames.erase(
                                source.frames.begin(), source.frames.begin() + source.frames.size() - 599);
                    }
                }
            }
            sender.sendMessage(filteredMessage, true);
        }
    }
    uint8_t count = 0;
    float xoffset = 40.f;
    for (sensor_source_t & source : sources) {
        float yoffset = 40.f + 180.f * count;
        long frameCount = source.frames.size() > 600 ? source.frames.size() - 600 : 0;
        float tickSize = (ofGetWindowWidth() - 2.f * xoffset) / 600.f;
        for (long f = source.frames.size() - 1; f >= frameCount ; --f) {
            sensor_frame_t frame = source.frames[f];
            for (int i = 0; i < frame.data.size(); ++i) {
                if (i < source.paths.size()) {
                    float ypos;
                    if (i == 0) {
                        ypos = 80.f * frame.data[i] / 360.f - 40.f;
                    } else if (i > 0 && i < 3) {
                        ypos = 40.f * frame.data[i] / 180.f;
                    } else {
                        ypos = frame.data[i] * 2.f;
                    }
                    source.paths[i].lineTo(xoffset + (600 - (f - frameCount)) * tickSize,
                            yoffset + 20.f + (i < 3 ? 40.f : 50.f) + ypos);
                }
            }
        }
        ++count;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
#ifdef USE_VIDEO
    videoGrabber.draw(20.0, 20.0);
#endif

    uint8_t count = 0;
    for (sensor_source_t & source : sources) {
        float xoffset = 40.f;
        float yoffset = 40.f + 200.f * count;
        ofDrawBitmapString("/" + source.type + "/" + source.id, xoffset, yoffset);
        string data = "";
        if (source.frames.size() > 0) {
            /*
            data += "DATA: ";
            for (float &f : source.frames[source.frames.size() - 1].data) {
                data += ofToString(f, 2, 6, ' ') + " ";
            }

            data += "STATE: ";
            for (char &c : source.frames[source.frames.size() - 1].calibration) {
                if (c == 0x1) {
                    data += "1";
                } else if (c == 0x2) {
                    data += "2";
                } else if (c == 0x3) {
                    data += "3";
                } else {
                    data += "0";
                }
            }
            ofDrawBitmapString(data, xoffset + 100.f, yoffset);

            for (ofPath & path : source.paths) {
                path.draw();
                path.clear();
            }
             */
        }
        ++count;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::exit() {
    settings.save("settings.xml");
}

