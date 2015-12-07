#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "stuff.h"
#include "outline.h"
#include "asteroid.h"
#include "sprite.h"

class Circle : public ofxBox2dCircle, public Stuff {
public:
    Circle() : ofxBox2dCircle() {};
    virtual ~Circle() {};
    virtual void update() {};
    virtual void draw() {
        ofPushStyle();
        ofFill();
        ofSetHexColor(0xf6c738);
        ofxBox2dCircle::draw();
        ofPopStyle();
    };
};

class Box : public ofxBox2dRect, public Stuff {
public:
    Box() : ofxBox2dRect() {};
    virtual ~Box() {};
    virtual void update() {};
    virtual void draw() {
        ofPushStyle();
        ofFill();
        ofSetHexColor(0xBF2545);
        ofxBox2dRect::draw();
        ofPopStyle();
    };
};


class World {
private:
    ofxBox2d box2d;
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

        float w = gui.getWidth();
        gui.add( clearBtn.setup("Clear", w, 28) );
        clearBtn.addListener(this, &World::clear);
        gui.add( wakeUpBtn.setup("Wake up", w, 28) );
        wakeUpBtn.addListener(this, &World::wakeUp);

        gui.add( numBodies );
        gui.add( numJoints );

        gui.add( paused );
        gui.add( debug );
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
        shared_ptr<Circle> obj = shared_ptr<Circle>(new Circle);
        obj->setPhysics(1.0, 0.2, 0.2);
        obj->setup(getB2World(), x, y, r);
        obj->setVelocity(ofRandom(-10, 10), ofRandom(-10, 10));
        add(obj);
    };

    void addRect(int x, int y) {
        float w = ofRandom(4, 20);
        float h = ofRandom(4, 20);
        shared_ptr<Box> obj = shared_ptr<Box>(new Box);
        obj->setPhysics(3.0, 0.53, 0.1);
        obj->setup(box2d.getWorld(), x, y, w, h);
        add(obj);
    };

    void addAsteroid(int x, int y) {
        shared_ptr<Asteroid> ast = shared_ptr<Asteroid>(new Asteroid);
        ast->setup(getB2World(), x, y);
        add(ast);
    };

    SpritePtr addSprite(float x, float y, string name, float s=30, float scale = 1.0, float d=1, float b=1, float f=1) {
        string fname("sprites/");
        fname += name;
        SpritePtr spr = SpritePtr(new Sprite);
        spr->setup(getB2World(), x, y, fname, s, scale, d, b, f);
        if (debug) {
            spr->showDebug = debug;
        }
        add(spr);
        return spr;
    };

    BoxSpritePtr addBoxSprite(float x, float y, string name, float s=1.0, float ss = 1.0, float d=1, float b=1, float f=1) {
        string fname("sprites/");
        fname += name;
        BoxSpritePtr spr = BoxSpritePtr(new BoxSprite);
        spr->setup(getB2World(), x, y, fname, s, ss, d, b, f);
        if (debug) {
            spr->showDebug = debug;
        }
        add(spr);
        return spr;
    };
};
