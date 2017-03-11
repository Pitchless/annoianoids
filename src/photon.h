
#pragma once

#include "ofxBox2d.h"
#include "ofxBox2dCircle.h"
#include "ofxBox2dBaseShape.h"
#include "stuff.h"

class Photon : public ofxBox2dBaseShape, public Stuff {
private:
    float radius;
public:
    Photon() : radius(2.0) {};
    virtual ~Photon() {};
    
    virtual void setup(b2World *world, float x, float y) {
        // these are used to create the shape
        b2CircleShape shape;

        shape.m_p.SetZero();
        shape.m_radius		= radius / OFX_BOX2D_SCALE;

        fixture.shape		= &shape;
        fixture.density		= 0.6;
        fixture.friction	= 0.4;
        fixture.restitution	= 1.0;

        bodyDef.type	= b2_dynamicBody;

        bodyDef.position.Set(x/OFX_BOX2D_SCALE, y/OFX_BOX2D_SCALE);

        body  = world->CreateBody(&bodyDef);
        body->CreateFixture(&fixture);

        alive = true;
    };

    virtual void update() {};

    virtual void draw() {
        if(!isBody()) return;

        ofPushMatrix();
        ofPushStyle();
        ofEnableAlphaBlending();
        ofTranslate(getPosition().x, getPosition().y, 0);
        ofRotate(getRotation(), 0, 0, 1);
        ofFill();
        ofSetColor(255,255,255,200);
        ofDrawCircle(0, 0, radius);
        ofSetColor(255,255,0,223);
        ofDrawCircle(0, 0, radius/2);
        ofPopStyle();
        ofPopMatrix();

    };
};

typedef shared_ptr<Photon> PhotonPtr;
