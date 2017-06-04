//
// Created by anton on 04/06/17.
//

#ifndef PULSATIONS_LAYOUT_H
#define PULSATIONS_LAYOUT_H

#include "ofMain.h"
#include "ofxXmlSettings.h"

class Layout {
public:
    Layout();

    void init(string name);

    void load(string name);
    void save();

    ofxXmlSettings get();

private:
    string _name;
    ofxXmlSettings _layout;
};


#endif //PULSATIONS_LAYOUT_H
