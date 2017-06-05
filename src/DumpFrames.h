//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_DUMPFRAMES_H
#define PULSATIONS_DUMPFRAMES_H

#include "ofMain.h"
#include "Sensor.h"

class DumpFrames {
public:
    void dump(string filename, Sensor *sensor) {
        if (sensor->getFrameBuffer().size() == 0) {
            return;
        }
        ofFile file;
        file.open(filename, ofFile::WriteOnly);
        ofBuffer out;
        for (sensor_frame_t &frame : sensor->getFrameBuffer()) {
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
    }
};


#endif //PULSATIONS_DUMPFRAMES_H
