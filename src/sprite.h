
#pragma once

#include "ofxBox2d.h"
#include "ofxBox2dCircle.h"
#include "ofxBox2dBaseShape.h"
#include "World.h"
#include "stuff.h"

class Sprite;
typedef shared_ptr<Sprite> SpritePtr;

class Sprite : public ofxBox2dCircle, public Stuff {
private:
    ofImage         image;
    ofColor         tint;
    float shapeScale;
public:
    ofParameter<bool> showDebug;
    Sprite() : tint(255,255,255,255), shapeScale(1.0), showDebug("Show Debug", false) {
      // TODO: Why doesn't this work?
      //ofLogNotice() << "HELLO";
      //density = 1.0;
      //friction = 1.0;
      //bounce = 1.0;
    };
    virtual ~Sprite() {};

    //virtual void setup(b2World *world, float x, float y) {
    virtual void setup(b2World *world, float x, float y, string fname, float scale=1.0, float d=1.0, float b=1.0, float f=1.0) {
        shapeScale = scale;
        setPhysics(d, b, f);
        ofxBox2dCircle::setup(world, x, y, 40);
        if (image.loadImage(fname)) {
            ofLogNotice() << "Loaded texture file '" << fname << "'";
        } else {
            ofLogError() << "Failed to load texture file '" << fname << "'";
        }
    };

    virtual void update() {};

    virtual void draw() {
        if(!isBody()) return;

        float radius = getRadius();
        float x = getPosition().x;
        float y = getPosition().y;
        float scale = (image.getHeight()/(radius*2))/shapeScale;
        float w = image.getWidth()/scale;
        float h = image.getHeight()/scale;

	ofPushStyle();
        glPushMatrix();
        glTranslatef(x, y, 0);
        glRotatef(getRotation(), 0, 0, 1);
        ofSetColor(255,255,255); // No tint
        image.draw(-(w/2), -(h/2), w, h);
	if (showDebug) {
            ofEnableAlphaBlending();
            ofFill();
            ofSetColor(0,255,0,120);
            ofCircle(0, 0, radius);
	}
        glPopMatrix();
	ofPopStyle();
    };
};
