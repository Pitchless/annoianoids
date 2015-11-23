
#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "Style.h"

static int hexColors[4] = {0x31988A, 0xFDB978, 0xFF8340, 0xE8491B};

class Asteroid {
private:
    ofImage         texture;
    ofMesh          mesh;
    ofColor         color;
    ofxBox2dPolygon polyShape;
    Style style;

public:
    Asteroid()
        : style(STYLE_OLDSKOOL)
    {
        // TODO? Allocate empty image?
    };

    void setup(b2World *world, float cx, float cy, float r) {
        ofDisableArbTex();
        string fname("tex_6.png");
        if (texture.loadImage(fname)) {
            ofLogNotice() << "Loaded texture file '" << fname << "'";
        } else {
            ofLogError() << "Failed to load texture file '" << fname << "'";
        }
        //printf("IMAGE %f x %f\n", texture.getWidth(), texture.getHeight());
        texture.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        texture.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);

        color.setHex(hexColors[(int)ofRandom(4)]);
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        int   nPts  = 8;
        float scale = r / (float)texture.getWidth();
        for (int i=0; i<nPts; i++) {
            float n = ofMap(i, 0, nPts-1, 0.0, TWO_PI);
            float x = cos(n);
            float y = sin(n);
            float d = ofRandom(-r/2, r/2);
            polyShape.addVertex(ofPoint(cx + (x * r + d), cy + (y * r + d)));
            mesh.addTexCoord(ofPoint(0, 0));
            mesh.addTexCoord(ofPoint(x * scale, y * scale));
        }
        polyShape.setPhysics(2.0, 0.8, 0.1);
        polyShape.create(world);

    };

    void update() {
    };

    void draw() {
        ofPushStyle();
        ofPushMatrix();
        ofTranslate(getPosition());
        ofRotate(polyShape.getRotation(), 0, 0, 1);
        if (style == STYLE_TEX) {
	  drawTex();
	} else {
        // Old skool
	    ofNoFill();
	    ofEnableAlphaBlending();
	    ofSetLineWidth(7);
	    ofSetColor(ofColor::blueSteel, 0.1);
	    ofBeginShape();
	      vector<ofPoint> pts = polyShape.getPoints();
	      for (int i=0; i<pts.size(); i++) {
		  ofVertex(pts[i]);
	      }
	    ofEndShape(true);
	    ofSetLineWidth(3);
	    ofSetColor(ofColor::white);
	    ofBeginShape();
	      pts = polyShape.getPoints();
	      for (int i=0; i<pts.size(); i++) {
		  ofVertex(pts[i]);
	      }
	    ofEndShape(true);
	}
        ofPopMatrix();
	ofPopStyle();
    }
    void drawTex() {
        ofPushStyle();
        mesh.clearVertices();
        vector<ofPoint> &pts = polyShape.getPoints();
        ofPoint center       = polyShape.getCentroid2D();
        for (int i=0; i<pts.size(); i++) {
            mesh.addVertex(center);
            mesh.addVertex(pts[i]);
        }
        mesh.addVertex(center);
        mesh.addVertex(pts.front());

        ofSetColor(color);
        texture.bind();
        mesh.draw();
        texture.unbind();
        ofPopStyle();

        stringstream info;
        ofVec2f pos = getPosition();
        info << "Pos: " << pos.x << " x " << pos.y;
        ofSetColor(ofColor::green);
        //ofRectangle box = it->getBoundingBox();
        //ofRect(box);
        //ofDrawBitmapString(info.str(), box.x, box.y);
        ofDrawBitmapString(info.str(), pos.x, pos.y);
    };

    ofVec2f getPosition() {
        return polyShape.getPosition();
    };
    void setPosition(ofVec2f pos) {
        polyShape.setPosition(pos);
    };
    void setPosition(float x, float y) {
        polyShape.setPosition(x, y);
    };
};

