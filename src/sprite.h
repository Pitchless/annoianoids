
#pragma once

#include "ofxBox2d.h"
#include "ofxBox2dCircle.h"
#include "ofxBox2dBaseShape.h"
#include "World.h"
#include "stuff.h"

class Sprite;
typedef shared_ptr<Sprite> SpritePtr;

class Sprite : public ofxBox2dCircle {
private:
    ofImage         image;
    ofColor         color;
public:
    Sprite() {};
    ~Sprite() {};

    //virtual void setup(b2World *world, float x, float y) {
    void setup(b2World *world, float x, float y, string fname) {
        setPhysics(6.0, 1.00, 0.8);
        ofxBox2dCircle::setup(world, x, y, 40);
        if (image.loadImage(fname)) {
            ofLogNotice() << "Loaded texture file '" << fname << "'";
        } else {
            ofLogError() << "Failed to load texture file '" << fname << "'";
        }
    };

    virtual void draw() {
        if(!isBody()) return;

        /*
        float radius = getRadius();
            ofPushMatrix();
            ofPushStyle();
            ofEnableAlphaBlending();
            ofTranslate(getPosition().x, getPosition().y, 0);
            ofRotate(getRotation(), 0, 0, 1);
            ofFill();
            ofSetColor(255,255,255,200);
            ofCircle(0, 0, radius);
            ofSetColor(255,255,0,223);
            ofCircle(0, 0, radius/2);
            ofPopStyle();
            ofPopMatrix();
        */
        float shapeScale = 1.0;
        float radius = getRadius();
        float x = getPosition().x;
        float y = getPosition().y;
        float scale = (image.getHeight()/(radius*2))/shapeScale;
        float w = image.getWidth()/scale;
        float h = image.getHeight()/scale;

        glPushMatrix();
        glTranslatef(x, y, 0);
        glRotatef(getRotation(), 0, 0, 1);
        ofSetColor(255,255,255); // No tint
        image.draw(-(w/2), -(h/2), w, h);
        glPopMatrix();
    };
};
