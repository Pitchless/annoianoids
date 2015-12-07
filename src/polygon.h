
#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "stuff.h"

class Polygon;
typedef shared_ptr<Polygon> PolygonPtr;

class Polygon : public ofxBox2dPolygon, public Stuff {
private:
    ofImage         texture;
    ofMesh          mesh;
    ofColor         color;

public:
    Polygon() {};
    virtual ~Polygon() {};

    void setup(b2World *world, float cx, float cy, string fname, float r=64) {
        if (r == 0)
            r = ofRandom(32, 64);
        ofDisableArbTex();
        if (texture.loadImage(fname)) {
            ofLogNotice() << "Loaded texture file '" << fname << "'";
        } else {
            ofLogError() << "Failed to load texture file '" << fname << "'";
        }
        //printf("IMAGE %f x %f\n", texture.getWidth(), texture.getHeight());
        texture.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        texture.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);

        color.set(255, 255, 255, 255);
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        int   nPts  = 4;
        float scale = r / (float)texture.getWidth();
        for (int i=0; i<nPts; i++) {
            float n = ofMap(i, 0, nPts-1, 0.0, TWO_PI);
            float x = cos(n);
            float y = sin(n);
            float d = ofRandom(-r/2, r/2);
            addVertex(ofPoint(cx + (x * r + d), cy + (y * r + d)));
            mesh.addTexCoord(ofPoint(0, 0));
            mesh.addTexCoord(ofPoint(x * scale, y * scale));
        }
        setPhysics(2.0, 0.8, 0.1);
        create(world);

    };

    void update() {};

    void draw() {
        ofPushStyle();
        ofPushMatrix();
            ofTranslate(getPosition());
            ofRotate(getRotation(), 0, 0, 1);
        mesh.clearVertices();
        vector<ofPoint> &pts = getPoints();
        ofPoint center       = getCentroid2D();
        for (int i=0; i<pts.size(); i++) {
            mesh.addVertex(center);
            mesh.addVertex(pts[i]);
        }
        mesh.addVertex(center);
        mesh.addVertex(pts.front());

        //ofSetColor(color);
        ofSetColor(255, 255, 255, 255);
        texture.bind();
        mesh.draw();
        texture.unbind();
        ofPopMatrix();
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
};

