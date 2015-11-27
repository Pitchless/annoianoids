#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "stuff.h"
#include "Outline.h"
#include "Asteroid.h"
#include "sprite.h"

class World {
private:
    ofxBox2d box2d;
    vector <shared_ptr<ofxBox2dCircle> > circles;
    vector <shared_ptr<ofxBox2dRect> > boxes;
    vector <shared_ptr<Outline> > outlines;
    Thing things;

public:
    ofParameter<int> numBodies, numJoints;
    ofParameter<float> gravityX;
    ofParameter<float> gravityY;
    ofParameter<float> border;
    ofParameter<bool> paused;
    ofParameter<bool> debug;
    float width, height;

    World()
        : numBodies("Num Bodies", 0, 0, 1000)
        , numJoints("Num Joints", 0, 0, 1000)
        , gravityX("Gravity X", 0, -2, 2)
        , gravityY("Gravity Y", 0, -10, 10)
        , border("World Border", 100, 0, 100)
        , paused("Pause", false)
        , debug("Debug", false)
    {};

    void setup(float w, float h) {
        width = w;
        height = h;
        numBodies.set("Num Bodies", 0, 0, 1000);
        box2d.init();
        box2d.setGravity(gravityX, gravityY);
        //box2d.createBounds(0,0,w,h+1000);
	box2d.createGround(0,h,w,h);
        box2d.setFPS(60.0);
        box2d.registerGrabbing();

        gravityX.addListener(this, &World::setGravityX);
        gravityY.addListener(this, &World::setGravityY);
    };

    ofxPanel gui;
    ofxButton clearBtn, wakeUpBtn, addBtn;
    void setupGui() {
        gui.setup("Box2DWorld");
        gui.add( numBodies );
        gui.add( numJoints );
        float w = gui.getWidth();
        gui.add( clearBtn.setup("Clear", w, 28) );
        clearBtn.addListener(this, &World::clear);
        gui.add( wakeUpBtn.setup("Wake up", w, 28) );
        gui.add( paused );
        gui.add( debug );
        wakeUpBtn.addListener(this, &World::wakeUp);
        gui.add( gravityX );
        gui.add( gravityY );
    };

    b2World* getB2World() {
        return box2d.getWorld();
    };

    void setGravityX(float &v) {
        box2d.setGravity(v, gravityY);
    };
    void setGravityY(float &v) {
        box2d.setGravity(gravityX, v);
    };
    void setGravity(float &x, float &y) {
        box2d.setGravity(x, y);
    };

    void update() {
        if (paused) {
            return;
        }
        box2d.update();
        things.update();
        float left = 0 - border;
        float right = width + border;
        float top = 0 - border;
        float bottom = height + border;
	/*
        for(int i=0; i<outlines.size(); i++) {
            outlines[i].get()->update();
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
        */
        // TODO: Cull (destroy) objects that have gone too far and reached the outer limits.
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

        //for(int i=0; i<asteroids.size(); i++) {
        //    asteroids[i].get()->draw();
        //}
        things.draw();

        for(int i=0; i<outlines.size(); i++) {
            outlines[i].get()->draw();
        }

        box2d.draw();
    };

    void clear() {
        circles.clear();
        boxes.clear();
        outlines.clear();
        //asteroids.clear();
        things.clear();
    };

    void wakeUp() {
        box2d.wakeupShapes();
    }

    void add(StuffPtr stuff) {
        //stuff->create(getB2World());
        things.add(stuff);
    };

    void addCircle(int x, int y) {
        float r = ofRandom(10, 23);
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
        shared_ptr<Asteroid> ast = shared_ptr<Asteroid>(new Asteroid);
        ast->setup(getB2World(), x, y);
        add(ast);
    };

    SpritePtr addSprite(float x, float y, string name, float scale = 1.0, float d=1, float b=1, float f=1) {
        string fname("sprites/");
        fname += name;
        SpritePtr spr = SpritePtr(new Sprite);
        //spr->setPhysics(density, bounce, friction);
        spr->setup(getB2World(), x, y, fname, scale, d, b, f);
        if (debug) {
            spr->showDebug = debug;
        }
        add(spr);
        return spr;
    };
};
