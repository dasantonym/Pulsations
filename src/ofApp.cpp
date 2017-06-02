#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);

    settings.load("settings.xml");

    isRecording = false;
    isLoop = false;
    drawCurves = (bool) settings.getValue("ui:drawCurves", true);

    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gui->addHeader(":: PULSATIONS ::");

    gui->addFRM();

    gui->addToggle("Draw curves", drawCurves);
    gui->addToggle("Record data", false);

    ofxDatGuiFolder *osc = gui->addFolder("OSC", ofColor::white);
    osc->addTextInput("Input port", ofToString(settings.getValue("osc:inputPort", 8888)));
    osc->addTextInput("Forward IP", settings.getValue("osc:forwardIP", "127.0.0.1"));
    osc->addTextInput("Forward port", ofToString(settings.getValue("osc:forwardPort", 9999)));

    ofxDatGuiFolder *sensor1 = gui->addFolder("BNO055 Sensor 1", ofColor::yellow);
    sensor1->addTextInput("Sensor ID 1", settings.getValue("sensor:id1:sid", "100"));
    sensor1->addSlider("Trigger ACC 1", 0.f, 50.f, (float) settings.getValue("sensor:id1:accelerationThreshold", 6.f));
    sensor1->addSlider("Avg 1s 1", -10.f, 10.f, 0.0);
    // sensor1->addValuePlotter("Avg 1s 1", -10.f, 10.f)->setDrawMode(ofxDatGuiGraph::LINES);

    ofxDatGuiFolder *sensor2 = gui->addFolder("BNO055 Sensor 2", ofColor::yellow);
    sensor2->addTextInput("Sensor ID 2", settings.getValue("sensor:id2:sid", "101"));
    sensor2->addSlider("Trigger ACC 2", 0.f, 50.f, (float) settings.getValue("sensor:id2:accelerationThreshold", 6.f));
    sensor2->addSlider("Avg 1s 2", -10.f, 10.f, 0.f);
    //sensor2->addValuePlotter("Avg 1s 2", -10.f, 10.f)->setDrawMode(ofxDatGuiGraph::LINES);

    ofxDatGuiFolder *sensor3 = gui->addFolder("BNO055 Sensor 3", ofColor::yellow);
    sensor3->addTextInput("Sensor ID 3", settings.getValue("sensor:id3:sid", "102"));
    sensor3->addSlider("Trigger ACC 3", 0.f, 50.f, (float) settings.getValue("sensor:id3:accelerationThreshold", 6.f));
    sensor3->addSlider("Avg 1s 3", -10.f, 10.f, 0.0);
    //sensor3->addValuePlotter("Avg 1s 3", -10.f, 10.f)->setDrawMode(ofxDatGuiGraph::LINES);

    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);

    sender.setup(settings.getValue("osc:forwardIP", "127.0.0.1"), settings.getValue("osc:forwardPort", 9999));
    receiver.setup(settings.getValue("osc:inputPort", 8888));

#ifdef USE_VIDEO
    videoGrabber.setDeviceID(0);
    videoGrabber.setDesiredFrameRate(25);
    videoGrabber.setup(1280, 720);

    videoRecorder.setup(tstamp + ".mp4", 1280, 720, 25);
    videoRecorder.start();
#endif

    for (int i = 0; i < 3; ++i) {
        string valuePath = "sensor:id" + ofToString(i+1);
        sensor_source_t source;
        source.id = settings.getValue(valuePath + ":sid", "10" + ofToString(i));
        source.type = "bno055";
        source.name = "BNO 055 IMU Fusion Sensor";
        source.startTime = 0;
        source.settings.active = true;
        source.settings.accelerationThreshold = (float) settings.getValue(valuePath + ":accelerationThreshold", 6.f);
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
    } else if (e.target->getLabel() == "Draw curves") {
        drawCurves = !drawCurves;
        settings.setValue("ui:drawCurves", drawCurves);
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
    } else if (e.target->getLabel() == "Sensor ID 1") {
        settings.setValue("sensor:id1:sid", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Sensor ID 2") {
        settings.setValue("sensor:id2:sid", ofToInt(e.target->getText()));
    } else if (e.target->getLabel() == "Sensor ID 3") {
        settings.setValue("sensor:id3:sid", ofToInt(e.target->getText()));
    }
}

//--------------------------------------------------------------
void ofApp::updateStats(uint8_t sourceId){
    while (sources[sourceId].stats.accelerationValues.size() > 0 && ofGetSystemTime() - sources[sourceId].stats.accelerationValues[0].w > sources[sourceId].stats.bufferTimeMillis) {
        sources[sourceId].stats.accelerationValues.erase(sources[sourceId].stats.accelerationValues.begin(), sources[sourceId].stats.accelerationValues.begin() + 1);
    }
    while (sources[sourceId].stats.orientationValues.size() > 0 && ofGetSystemTime() - sources[sourceId].stats.orientationValues[0].w > sources[sourceId].stats.bufferTimeMillis) {
        sources[sourceId].stats.orientationValues.erase(sources[sourceId].stats.orientationValues.begin(), sources[sourceId].stats.orientationValues.begin() + 1);
    }
    for (ofVec4f & vec : sources[sourceId].stats.accelerationValues) {
        sources[sourceId].stats.accelerationAvg.x += vec.x;
        sources[sourceId].stats.accelerationAvg.y += vec.y;
        sources[sourceId].stats.accelerationAvg.z += vec.z;
        if (vec.x > sources[sourceId].stats.accelerationMax.x) {
            sources[sourceId].stats.accelerationMax.x = vec.x;
        }
        if (vec.y > sources[sourceId].stats.accelerationMax.y) {
            sources[sourceId].stats.accelerationMax.y = vec.y;
        }
        if (vec.z > sources[sourceId].stats.accelerationMax.z) {
            sources[sourceId].stats.accelerationMax.z = vec.z;
        }
        if (vec.x < sources[sourceId].stats.accelerationMin.x) {
            sources[sourceId].stats.accelerationMin.x = vec.x;
        }
        if (vec.y < sources[sourceId].stats.accelerationMin.y) {
            sources[sourceId].stats.accelerationMin.y = vec.y;
        }
        if (vec.z < sources[sourceId].stats.accelerationMin.z) {
            sources[sourceId].stats.accelerationMin.z = vec.z;
        }
    }

    if (sources[sourceId].stats.accelerationMax.x > sources[sourceId].stats.accelerationMaxGlobal) {
        sources[sourceId].stats.accelerationMaxGlobal = sources[sourceId].stats.accelerationMax.x;
    }
    if (sources[sourceId].stats.accelerationMax.y > sources[sourceId].stats.accelerationMaxGlobal) {
        sources[sourceId].stats.accelerationMaxGlobal = sources[sourceId].stats.accelerationMax.y;
    }
    if (sources[sourceId].stats.accelerationMax.y > sources[sourceId].stats.accelerationMaxGlobal) {
        sources[sourceId].stats.accelerationMaxGlobal = sources[sourceId].stats.accelerationMax.y;
    }
    if (sources[sourceId].stats.accelerationMin.x < sources[sourceId].stats.accelerationMinGlobal) {
        sources[sourceId].stats.accelerationMinGlobal = sources[sourceId].stats.accelerationMin.x;
    }
    if (sources[sourceId].stats.accelerationMin.y > sources[sourceId].stats.accelerationMinGlobal) {
        sources[sourceId].stats.accelerationMinGlobal = sources[sourceId].stats.accelerationMin.y;
    }
    if (sources[sourceId].stats.accelerationMin.y > sources[sourceId].stats.accelerationMinGlobal) {
        sources[sourceId].stats.accelerationMinGlobal = sources[sourceId].stats.accelerationMin.y;
    }

    sources[sourceId].stats.accelerationAvg.x = sources[sourceId].stats.accelerationValues.size() ? sources[sourceId].stats.accelerationAvg.x / (float) sources[sourceId].stats.accelerationValues.size() : 0.f;
    sources[sourceId].stats.accelerationAvg.y = sources[sourceId].stats.accelerationValues.size() ? sources[sourceId].stats.accelerationAvg.y / (float) sources[sourceId].stats.accelerationValues.size() : 0.f;
    sources[sourceId].stats.accelerationAvg.z = sources[sourceId].stats.accelerationValues.size() ? sources[sourceId].stats.accelerationAvg.z / (float) sources[sourceId].stats.accelerationValues.size() : 0.f;

    sources[sourceId].stats.accelerationAvgGlobal = ( sources[sourceId].stats.accelerationAvg.x + sources[sourceId].stats.accelerationAvg.y + sources[sourceId].stats.accelerationAvg.z) / 3.f;
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
        receiver.getNextMessage(msg);
        vector<string> address = ofSplitString(msg.getAddress(), "/", true, true);
        if (address.size() == 2 && address[0] == "bno055") {
            bool sendFrame = false;
            ofVec4f acceleration, orientation;
            filteredMessage.clear();
            filteredMessage.setAddress(msg.getAddress());
            filteredMessage.addTimetagArg(time_received);
            for (sensor_source_t & source : sources) {
                if (source.type == address[0] && source.id == address[1]) {
                    // FIXME: timetags are not received properly...
                    sensor_frame_t frame;
                    for (int i = 0; i < msg.getNumArgs(); ++i) {
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
                            if (i > 3 && i < 7) {
                                if (fabs(msg.getArgAsFloat(i)) >= source.settings.accelerationThreshold) {
                                    sendFrame = true;
                                    filteredMessage.addFloatArg(msg.getArgAsFloat(i));
                                }
                                frame.data.push_back(msg.getArgAsFloat(i));
                            } else {
                                filteredMessage.addFloatArg(msg.getArgAsFloat(i));
                                frame.data.push_back(msg.getArgAsFloat(i));
                            }
                        } else if (type == "b") {
                            frame.calibration = msg.getArgAsBlob(i);
                        } else if (type == "T") {
                            if (source.startTime == 0) {
                                source.startTime = time_received;
                            }
                            orientation.w = time_received;
                            acceleration.w = time_received;
                            source.lastTime = time_received;
                            frame.time = time_received;
                        }
                    }
                    source.stats.accelerationValues.push_back(acceleration);
                    source.stats.orientationValues.push_back(orientation);
                    source.frames.push_back(frame);
                }
            }
            if (sendFrame) {
                if (isLoop && sendFrame) {
                    loopMessages.push_back(filteredMessage);
                }
                sender.sendMessage(filteredMessage, true);
            }
        }
    }


    
    uint8_t count = 0;
    float xoffset = 40.f;
    for (sensor_source_t & source : sources) {
        updateStats(count);

        //gui->getValuePlotter("Avg 1s " + ofToString(count + 1))->setValue(source.stats.accelerationAvgGlobal);
        gui->getSlider("Avg 1s " + ofToString(count + 1))->setValue(source.stats.accelerationAvgGlobal);

        if (!isRecording && source.frames.size() > 600) {
            source.frames.erase(source.frames.begin(), source.frames.begin() + source.frames.size() - 600);
        }

        if (drawCurves) {
            float yoffset = 40.f + 180.f * count;
            long frameCount = source.frames.size() > 600 ? source.frames.size() - 600 : 0;
            float tickSize = (ofGetWindowWidth() - 2.f * xoffset) / 600.f;
            for (long f = source.frames.size() - 1; f >= frameCount ; --f) {
                sensor_frame_t frame = source.frames[f];
                for (int i = 0; i < frame.data.size(); ++i) {
                    if (i < source.paths.size()) {
                        float ypos;
                        if (i == 0) {
                            ypos = 80.f * (frame.data[i] / 360.f) - 40.f;
                        } else if (i > 0 && i < 3) {
                            ypos = 40.f * frame.data[i] / 180.f;
                        } else {
                            ypos = frame.data[i] * 2.f;
                        }
                        source.paths[i].lineTo(xoffset + (600 - (f - frameCount)) * tickSize,
                                yoffset + 70.f + ypos);
                    }
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
            for (float &f : source.frames[source.frames.size() - 1].data) {
                data += ofToString(f, 2, 6, ' ') + " ";
            }
            data += " ";
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

            if (drawCurves) {
                for (ofPath & path : source.paths) {
                    path.draw();
                    path.clear();
                }
            }
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
        case ' ':
            isLoop = !isLoop;
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

