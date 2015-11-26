
#pragma once

#include "ofMain.h"

/*
 * Class to handle the background, fills the window.
 */
class Background
{

public:
    ofParameter<ofColor> color1;
    ofParameter<ofColor> color2;

    Background() {
        color1.set( "BGColor1", ofColor ( 0,0,0,255 ),ofColor ( 0,0,0,0 ),ofColor ( 255,255,255,255 ) );
        color2.set( "BGColor2", ofColor ( 255,255,255,255 ),ofColor ( 0,0,0,0 ),ofColor ( 255,255,255,255 ) );
    };
    virtual ~Background(){};

    void setup() {};
    void update() {};

    void draw() {
        ofBackgroundGradient( color1, color2 );
    };
};
