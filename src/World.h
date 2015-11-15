#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "Outline.h"
#include "Asteroid.h"

class World {
private:
    ofxBox2d box2d;
    vector <shared_ptr<ofxBox2dCircle> > circles;
    vector <shared_ptr<ofxBox2dRect> > boxes;
    vector <shared_ptr<Outline> > outlines;
    vector <shared_ptr<Asteroid> > asteroids;

public:
    ofParameter<int> numBodies;
    ofParameter<float> gravityX;
    ofParameter<float> gravityY;

    void setup(float w, float h) {
        numBodies.set("Num Bodies", 0, 0, 1000);
        gravityX.set("Gravity X", 0, -23, 23);
        gravityY.set("Gravity Y", 0, -23, 23);
        box2d.init();
        box2d.setGravity(gravityX, gravityY);
        box2d.createBounds(0,0,w,h);
        box2d.setFPS(60.0);
        box2d.registerGrabbing();

        gravityX.addListener(this, &World::setGravityX);
        gravityY.addListener(this, &World::setGravityY);
    };
    
    void setGravityX(float &v) { gravityX = v; box2d.setGravity(gravityX, gravityY);};
    void setGravityY(float &v) { gravityY = v; box2d.setGravity(gravityX, gravityY);};

    void update() {
        box2d.update();
        for(int i=0; i<outlines.size(); i++) {
            outlines[i].get()->update();
        }
        numBodies = box2d.getBodyCount();
    };

    void updateOutlines(vector<ofPolyline> blobs) {
        outlines.clear();
        for ( int i=0; i<blobs.size(); i++) {
            shared_ptr<Outline> ol = shared_ptr<Outline>(new Outline);
            ol->setup(box2d.getWorld(), blobs[i]);
            outlines.push_back(ol);
        }
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

        for(int i=0; i<asteroids.size(); i++) {
            asteroids[i].get()->draw();
        }

        for(int i=0; i<outlines.size(); i++) {
            outlines[i].get()->draw();
        }

        box2d.draw();
    };

    void clear() {
        circles.clear();
        boxes.clear();
        outlines.clear();
        asteroids.clear();
    };

    void addCircle(int x, int y) {
        float r = ofRandom(4, 20);
        circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
        circles.back().get()->setPhysics(3.0, 0.53, 0.1);
        circles.back().get()->setup(box2d.getWorld(), x, y, r);
        circles.back().get()->setVelocity(ofRandom(-30, 30), ofRandom(-30, 30));
    };

    void addRect(int x, int y) {
        float w = ofRandom(4, 20);
        float h = ofRandom(4, 20);
        boxes.push_back(shared_ptr<ofxBox2dRect>(new ofxBox2dRect));
        boxes.back().get()->setPhysics(3.0, 0.53, 0.1);
        boxes.back().get()->setup(box2d.getWorld(), x, y, w, h);
    };

    void addAsteroid(int x, int y) {
        float r = ofRandom(20, 60);
        asteroids.push_back(shared_ptr<Asteroid>(new Asteroid));
        asteroids.back().get()->setup(box2d.getWorld(), x, y, r);
    };
};
