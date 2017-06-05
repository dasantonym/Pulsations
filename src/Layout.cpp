//
// Created by anton on 04/06/17.
//

#include "Layout.h"

Layout::Layout() {
}

void Layout::init(string name) {
    _layout.clear();
    _name = name;

    _layout.setValue("layout:sensorcount", 2);

    for (int i = 0; i < _layout.getValue("layout:sensorcount", 2); ++i) {
        string valuePath = "layout:sensor" + ofToString(i+1);

        _layout.setValue(valuePath + ":sid", "10" + ofToString(i));
        _layout.setValue(valuePath + ":name", "BNO 055 IMU Fusion Sensor");
        _layout.setValue(valuePath + ":type", "bno055");
        _layout.setValue(valuePath + ":triggercount", 1);

        for (int t = 0; t < _layout.getValue(valuePath + ":trigger", 1); ++t) {
            string tid = valuePath + ":trigger" + ofToString(t+1);
            _layout.setValue(tid + ":target", "acceleration");
            _layout.setValue(tid + ":name", "abs" + ofToString(i+1));
            _layout.setValue(tid + ":range", i);
            _layout.setValue(tid + ":absolute", 1);
            _layout.setValue(tid + ":debounce", 500);
            _layout.setValue(tid + ":falloff:x", .0f);
            _layout.setValue(tid + ":falloff:y", .0f);
            _layout.setValue(tid + ":falloff:z", .0f);

            _layout.setValue(tid + ":low:x", 6.f);
            _layout.setValue(tid + ":midi:x:channel", 1 + t * 3);
            _layout.setValue(tid + ":midi:x:pitch", .5f);
            _layout.setValue(tid + ":midi:x:duration", 250);

            _layout.setValue(tid + ":low:y", 6.f);
            _layout.setValue(tid + ":midi:y:channel", 2 + t * 3);
            _layout.setValue(tid + ":midi:y:pitch", .5f);
            _layout.setValue(tid + ":midi:y:duration", 250);

            _layout.setValue(tid + ":low:z", 6.f);
            _layout.setValue(tid + ":midi:z:channel", 3 + t * 3);
            _layout.setValue(tid + ":midi:z:pitch", .5f);
            _layout.setValue(tid + ":midi:z:duration", 250);

            if (_layout.getValue(tid + ":range", 0)) {
                _layout.setValue(tid + ":high:x", 10.f);
                _layout.setValue(tid + ":high:y", 10.f);
                _layout.setValue(tid + ":high:z", 10.f);
            }
        }
    }
}

void Layout::load(string name) {
    _layout.load("layout-" + name + ".xml");
    _name = name;
}

void Layout::save() {
    _layout.save("layout-" + _name + ".xml");
}

ofxXmlSettings Layout::get() {
    return _layout;
}
