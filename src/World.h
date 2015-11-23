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
    ofParameter<float> border;
    float width, height;
    ofxPanel gui;


    void setup(float w, float h) {
        width = w;
        height = h;
        numBodies.set("Num Bodies", 0, 0, 1000);
        gravityX.set("Gravity X", 0, -2, 2);
        gravityY.set("Gravity Y", 0, -10, 10);
        border.set("World Border", 100, 0, 100);
        box2d.init();
        box2d.setGravity(gravityX, gravityY);
        //box2d.createBounds(0,0,w,h);
        box2d.setFPS(60.0);
        box2d.registerGrabbing();

        gravityX.addListener(this, &World::setGravityX);
        gravityY.addListener(this, &World::setGravityY);
    };

    void setupGui() {
        gui.setup("Box2DWorld");
        gui.add( numBodies );
        gui.add( gravityX );
        gui.add( gravityY );
    }

    
    void setGravityX(float &v) { gravityX = v; box2d.setGravity(gravityX, gravityY);};
    void setGravityY(float &v) { gravityY = v; box2d.setGravity(gravityX, gravityY);};

    void update() {
        box2d.update();
        float left = 0 - border;
        float right = width + border;
        float top = 0 - border;
        float bottom = height + border;
        for(int i=0; i<outlines.size(); i++) {
            outlines[i].get()->update();
        }
        for(int i=0; i<asteroids.size(); i++) {
            Asteroid* ast = asteroids[i].get();
            ast->update();
            ofVec2f pos = ast->getPosition();
            if ( pos.x < left ) {
	        printf("Hello left %fx%f w:%f h:%f\n", pos.x, pos.y, left, right);
                pos.x = width;
	        printf("World %fx%f w:%f h:%f\n", pos.x, pos.y, left, right);
                ast->setPosition(pos);
            } else if ( pos.x > right ) {
	        printf("Hello right %fx%f w:%f h:%f\n", pos.x, pos.y, left, right);
                pos.x = 0;
                ast->setPosition(pos);
            } else if ( pos.y < top ) {
	        printf("Hello top %fx%f w:%f h:%f\n", pos.x, pos.y, top, bottom);
                pos.y = height;
                ast->setPosition(pos);
            } else if ( pos.y > bottom ) {
	        printf("Hello bot %fx%f w:%f h:%f\n", pos.x, pos.y, top, bottom);
                //pos.y = 0;
	        printf("World bot %fx%f w:%f h:%f\n", pos.x, pos.y, top, bottom);
                //ast->setPosition(foo);
                ast->setPosition(10.0,10.0);
            }
        }
        for(int i=0; i<circles.size(); i++) {
            ofxBox2dCircle* obj = circles[i].get();
            obj->update();
            ofVec2f pos = obj->getPosition();
            ofVec2f vel = obj->getVelocity();
            if ( pos.x < left ) {
                pos.x = width;
                obj->setPosition(pos);
                obj->setVelocity(vel);
            } else if ( pos.x > right ) {
                pos.x = 0;
                obj->setPosition(pos);
                obj->setVelocity(vel);
            } else if ( pos.y < top ) {
                pos.y = height;
                obj->setPosition(pos);
                obj->setVelocity(vel);
            } else if ( pos.y > bottom ) {
                pos.y = 0;
                obj->setPosition(pos);
                obj->setVelocity(vel);
            }
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
        float r = ofRandom(20, 40);
        circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
        circles.back().get()->setPhysics(6.0, 0.02, 0.2);
        circles.back().get()->setup(box2d.getWorld(), x, y, r);
        circles.back().get()->setVelocity(ofRandom(-10, 10), ofRandom(-10, 10));
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
