#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    isRecording = false;

    sender.setup("127.0.0.1", 9999);
    receiver.setup(8888);
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
        source.startTime = 0;
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
        sender.sendMessage(msg, true);
        vector<string> address = ofSplitString(msg.getAddress(), "/", true, true);
        if (address.size() == 2) {
            for (sensor_source_t & source : sources) {
                if (source.type == address[0] && source.id == address[1]) {
                    sensor_frame_t frame;
                    for (int i = 0; i < msg.getNumArgs(); ++i) {
                        string type = msg.getArgTypeName(i);
                        if (type == "f") {
                            frame.data.push_back(msg.getArgAsFloat(i));
                        } else if (type == "b") {
                            frame.calibration = msg.getArgAsBlob(i);
                        } else if (type == "T") {
                            // TODO: fix timetag
                            if (source.startTime == 0) {
                                source.startTime = time_received;
                            }
                            source.lastTime = time_received;
                            frame.time = time_received;
                        }
                    }
                    if (frame.data.size() == 6) {
                        if (isRecording) {
                            source.frames.push_back(frame);
                        }
                    }
                }
            }
        }
    }
    uint8_t count = 0;
    float xoffset = 40.f;
    for (sensor_source_t & source : sources) {
        float yoffset = 40.f + 110.f * count;
        long frameCount = source.frames.size() > 600 ? source.frames.size() - 600 : 0;
        float tickSize = (ofGetWindowWidth() - 2.f * xoffset) / 600.f;
        for (long f = source.frames.size() - 1; f >= frameCount ; --f) {
            sensor_frame_t frame = source.frames[f];
            for (int i = 0; i < frame.data.size(); ++i) {
                if (i < source.paths.size()) {
                    float ypos;
                    if (i == 0) {
                        ypos = 50.f * frame.data[i] / 360.f - 25.f;
                    } else if (i > 0 && i < 3) {
                        ypos = 25.f * frame.data[i] / 180.f;
                    } else {
                        ypos = frame.data[i] * 2.f;
                    }
                    source.paths[i].lineTo(xoffset + (600 - (f - frameCount)) * tickSize,
                            yoffset + 20.f + (i < 3 ? 25.f : 50.f) + ypos);
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

    ofDrawBitmapString(ofToString(ofGetFrameRate(), 2, 5, '0'), ofGetWindowWidth() - 140.f, 40.f);

    if (isRecording) {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("REC", ofGetWindowWidth() - 60.f, 40.f);
        ofPopStyle();
    }

    uint8_t count = 0;
    for (sensor_source_t & source : sources) {
        float xoffset = 40.f;
        float yoffset = 40.f + 110.f * count;
        ofDrawBitmapString(source.type + source.id, xoffset, yoffset);
        string calibration = "cal: ";
        if (source.frames.size() > 0) {
            for (char &c : source.frames[source.frames.size() - 1].calibration) {
                if (c == 0x1) {
                    calibration += "1 ";
                } else if (c == 0x2) {
                    calibration += "2 ";
                } else if (c == 0x3) {
                    calibration += "3 ";
                } else {
                    calibration += "0 ";
                }
            }
            ofDrawBitmapString(calibration, xoffset + 100.f, yoffset);

            for (ofPath & path : source.paths) {
                path.draw();
                path.clear();
            }
        }
        ++count;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case ' ':
            isRecording = !isRecording;
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
            break;
        case 'f':
            ofToggleFullscreen();
            break;

    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

