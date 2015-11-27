
#pragma once

#include "ofxBox2d.h"
#include "ofxBox2dCircle.h"
#include "ofxBox2dBaseShape.h"
#include "World.h"
#include "stuff.h"
#include "ofxGif.h"

class SpriteImage : public ofImage {
private:
    size_t idx;
    bool isGif;
    ofxGIF::fiGifLoader gifloader;

public:
    SpriteImage() : idx(0), isGif(false) {};
    virtual ~SpriteImage() {};
    
    bool load(const string& name) {
      if (ofFilePath::getFileExt(name) == "gif") {
	isGif = true;
        gifloader.load(name);
	return true;
      } else {
        return ofImage::load(name);
      }
    };

    float getWidth() { return isGif ? gifloader.pages[0].getWidth() : ofImage::getWidth(); };
    float getHeight() { return isGif ? gifloader.pages[0].getHeight() : ofImage::getHeight(); };
    
    void draw(float x, float y, float w, float h) {
      if (isGif) {
	  if (ofGetElapsedTimeMillis() % 3) {
	      idx++;
	      if (idx > gifloader.pages.size()-1) idx = 0;
	  }
          gifloader.pages[idx].draw(x, y, w, h);
      } else {
          ofImage::draw(x, y, w, h);
      }
    };
};

class Sprite;
typedef shared_ptr<Sprite> SpritePtr;

class Sprite : public ofxBox2dCircle, public Stuff {
private:
    SpriteImage         image;
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
    virtual void setup(b2World *world, float x, float y, string fname, float s=30, float scale=1.0, float d=1.0, float b=1.0, float f=1.0) {
        shapeScale = scale;
        setPhysics(d, b, f);
        ofxBox2dCircle::setup(world, x, y, s);
        if (image.load(fname)) {
            ofLogNotice() << "Loaded texture file hello '" << fname << "'";
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

class Box;
typedef shared_ptr<Box> BoxPtr;

class Box : public ofxBox2dRect, public Stuff {
private:
    SpriteImage         image;
    ofColor         tint;
    float shapeScale;
public:
    ofParameter<bool> showDebug;
    Box() : tint(255,255,255,255), shapeScale(1.0), showDebug("Show Debug", false) { };
    virtual ~Box() {};

    virtual void setup(b2World *world, float x, float y, string fname, float scale=1.0, float ss=1.0, float d=1.0, float b=1.0, float f=1.0) {
        shapeScale = ss;
        setPhysics(d, b, f);
        if (image.load(fname)) {
            ofLogNotice() << "Loaded texture file '" << fname << "'";
        } else {
            ofLogError() << "Failed to load texture file '" << fname << "'";
        }
        float w = image.getWidth()/scale;
        float h = image.getHeight()/scale;
	ofxBox2dRect::setup(world, x, y, w, h);
    };

    virtual void update() {};

    virtual void draw() {
        if(!isBody()) return;

        float x = getPosition().x;
        float y = getPosition().y;
        float w = getWidth();
        float h = getHeight();
	float r = getRotation();

	ofPushStyle();
        glPushMatrix();
        glTranslatef(x, y, 0);
        glRotatef(r, 0, 0, 1);
        ofSetColor(255,255,255); // No tint
        image.draw(-(w/2), -(h/2), w, h);
	if (showDebug) {
            ofEnableAlphaBlending();
            ofFill();
            ofSetColor(0,255,0,120);
	    ofDrawRectangle(-(w/2), -(h/2), w, h);
	}
        glPopMatrix();
	ofPopStyle();
    };
};