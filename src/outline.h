#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

class Outline {
private:
    ofxBox2dEdge edge_;

public:
    ofParameter<float> lineWidth;
    ofParameter<ofColor> lineColor;
    ofParameter<bool> bFill;

    void setup(b2World *world, ofPolyline blobline) {
        edge_.clear();
        edge_.addVertexes(blobline);
        edge_.create(world);
        lineColor.set("Line Color", ofColor::yellow, ofColor(0,0), ofColor(255,255));
	lineWidth.set("Line Width", 6.0, 0.0, 60.0);
	bFill.set("Fill", true);
    };

    void update() {
    };

    void draw() {
        edge_.updateShape();
        ofPushStyle();
        ofSetColor(lineColor);
        ofSetLineWidth(lineWidth);
        if (bFill) ofFill();
        else ofNoFill();
        edge_.draw();
        ofPopStyle();
    };
};
