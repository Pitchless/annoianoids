#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

class World {
private:
    ofxBox2d box2d;
    vector <shared_ptr<ofxBox2dCircle> > circles;
    vector <shared_ptr<ofxBox2dRect> > boxes;

public:
    ofParameter<int> numBodies;

    void setup() {
        numBodies.set("Num Bodies", 0, 0, 1000);
        box2d.init();
        box2d.setGravity(0,10);
        box2d.createBounds();
        box2d.setFPS(60.0);
        box2d.registerGrabbing();
    };

    void update() {
        box2d.update();
        numBodies = box2d.getBodyCount();
    };

    void draw() {
        for(int i=0; i<circles.size(); i++) {
            ofFill();
            ofSetHexColor(0xf6c738);
            circles[i].get()->draw();
        }

        for(int i=0; i<boxes.size(); i++) {
            ofFill();
            ofSetHexColor(0xBF2545);
            boxes[i].get()->draw();
        }

        box2d.draw();
    };

    void addCircle(int x, int y) {
        float r = ofRandom(4, 20);
        circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
        circles.back().get()->setPhysics(3.0, 0.53, 0.1);
        circles.back().get()->setup(box2d.getWorld(), x, y, r);
    };

    void addRect(int x, int y) {
        float w = ofRandom(4, 20);
        float h = ofRandom(4, 20);
        boxes.push_back(shared_ptr<ofxBox2dRect>(new ofxBox2dRect));
        boxes.back().get()->setPhysics(3.0, 0.53, 0.1);
        boxes.back().get()->setup(box2d.getWorld(), x, y, w, h);
    };
};
