//
// Created by anton on 04/06/17.
//

#include "SensorGraph.h"

SensorGraph::SensorGraph(ofPoint position, float width, float height) {
    _position = position;
    _size.x = width;
    _size.y = height;

    for (int i = 0; i < 3; ++i) {
        ofPath path;
        path.setFilled(false);
        path.setStrokeWidth(1.0f);
        ofColor color;
        switch (i) {
            case 0:
                path.setStrokeColor(color.cyan);
                break;
            case 1:
                path.setStrokeColor(color.magenta);
                break;
            case 2:
                path.setStrokeColor(color.yellow);
                break;
            case 3:
                path.setStrokeColor(color.red);
                break;
            case 4:
                path.setStrokeColor(color.green);
                break;
            case 5:
                path.setStrokeColor(color.blue);
                break;
            case 6:
                path.setStrokeColor(color.yellow);
                break;
            default:
                break;
        }
        _paths.push_back(path);
    }
}

void SensorGraph::update(vector<sensor_frame_t> *frames) {
    if (frames->size() == 0) {
        return;
    }
    float tickSize = (_size.x - _position.x) / frames->size();
    for (long f = 0; f < frames->size() ; f++) {
        sensor_frame_t frame = frames->at(f);
        for (int i = 0; i < _paths.size(); i++) {
            float ypos = .0f;
            switch (i) {
                case 0:
                    ypos = frame.acceleration.x;
                    break;
                case 1:
                    ypos = frame.acceleration.y;
                    break;
                case 2:
                    ypos = frame.acceleration.z;
                    break;
                default:
                    break;
            }
            _paths[i].lineTo(_position.x + f * tickSize, _position.y + _size.y * .5f + ypos * 2.f);
        }
    }
}

void SensorGraph::draw() {
    for (ofPath & path : _paths) {
        path.draw();
        path.clear();
    }
}
