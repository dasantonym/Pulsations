//
// Created by anton on 14/05/17.
//

#ifndef PULSATIONS_LIBLODUMPFILE_H
#define PULSATIONS_LIBLODUMPFILE_H

#include "ofMain.h"
#include "ofxOsc.h"

class LibloDumpFile {
public:
    bool open(string path) {
        ofLogVerbose() << "Opening liblo dump file at: " << path;
        _fin.open(path);
    };

    bool hasWaitingMessages() {
        try {
            getline(_fin, _nextLine);
            return true;
        } catch (ios_base::failure f) {
            return false;
        }
    }

    ofxOscMessage getNextMessage() {
        vector<string> values = ofSplitString(_nextLine, " ");

        ofxOscMessage oscMessage;
        if (values.size() > 0) {
            oscMessage.setAddress(values[0]);
        }

        if (values.size() > 1) {
            string types = values[1];
            long valueIndex = 2;
            for (char & c : types) {
                if (valueIndex <= types.size()) {
                    if (c == 'f') {
                        oscMessage.addFloatArg(ofToFloat(values[valueIndex]));
                    } else if (c == 'b') {
                        // TODO: implement blobs
                    } else if (c == 't') {
                        // TODO: implement timetags
                    }
                }
                ++valueIndex;
            }
        }

        return oscMessage;
    }

private:
    ifstream _fin;
    string _nextLine;
};


#endif //PULSATIONS_LIBLODUMPFILE_H
