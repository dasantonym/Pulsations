//
// Created by anton on 04/06/17.
//

#include "SensorGraph.h"

SensorGraph::SensorGraph(ofPoint position, float width, float height) {
    _position = position;
    _size.x = width;
    _size.y = height;

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
            default:
                break;
        }
        _paths.push_back(path);
    }
}

void SensorGraph::update(vector<sensor_frame_t> frames) {
    if (frames.size() == 0) {
        return;
    }
    float tickSize = (_size.x - _position.x) / frames.size();
    for (long f = frames.size() - 1; f >= frames.size() ; --f) {
        sensor_frame_t frame = frames[f];
        for (int i = 0; i < 6; ++i) {
            if (i < _paths.size()) {
                float ypos = _size.y * .5f;
                switch (i) {
                    case 0:
                        ypos = _size.y * (frame.orientation.x / 360.f) - _size.y * .5f;
                        break;
                    case 1:
                        ypos = _size.y * .5f / frame.orientation.y / 180.f;
                        break;
                    case 2:
                        ypos = _size.y * .5f / frame.orientation.z / 180.f;
                        break;
                    case 3:
                        ypos = frame.acceleration.x * 2.f;
                        break;
                    case 4:
                        ypos = frame.acceleration.y * 2.f;
                        break;
                    case 5:
                        ypos = frame.acceleration.z * 2.f;
                        break;
                }
                _paths[i].lineTo(_position.x + (_size.x - (f - frames.size())) * tickSize,
                        _position.y + _size.y * .5f + ypos);
            }
        }
    }
}

void SensorGraph::draw() {
    for (ofPath & path : _paths) {
        path.draw();
        path.clear();
    }
}