
#pragma once

#include "ofxBox2d.h"
#include "ofxBox2dCircle.h"
#include "ofxBox2dBaseShape.h"
#include "World.h"
#include "stuff.h"

class Photon : public Stuff {
private:
    float radius, x_, y_;
public:
    Photon() : radius(2.0), x_(0), y_(0) {};
    ~Photon() {};

    virtual void setup(float x, float y) {
        x_=x;
        y_=y;
    };

    virtual void create(b2World *world) {
        // these are used to create the shape
        b2CircleShape shape;

        shape.m_p.SetZero();
        shape.m_radius		= radius / OFX_BOX2D_SCALE;
        this->radius		= radius;

        fixture.shape		= &shape;
        fixture.density		= 0.6;
        fixture.friction	= 0.4;
        fixture.restitution	= 1.0;

        bodyDef.type	= b2_dynamicBody;

        bodyDef.position.Set(x_/OFX_BOX2D_SCALE, y_/OFX_BOX2D_SCALE);

        body  = world->CreateBody(&bodyDef);
        body->CreateFixture(&fixture);

        alive = true;
    };

    virtual void draw() {
        if(!isBody()) return;

        ofPushMatrix();
        ofPushStyle();
        ofEnableAlphaBlending();
        ofTranslate(getPosition().x, getPosition().y, 0);
        ofRotate(getRotation(), 0, 0, 1);
        ofFill();
        ofSetColor(223,223,223,223);
        ofCircle(0, 0, radius);
        ofSetColor(255,255,255,255);
        ofCircle(0, 0, radius/2);
        ofPopStyle();
        ofPopMatrix();

    };
};

typedef shared_ptr<Photon> PhotonPtr;
