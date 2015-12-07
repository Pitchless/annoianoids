
#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "stuff.h"

static int hexColors[4] = {0x31988A, 0xFDB978, 0xFF8340, 0xE8491B};

class Asteroid : public ofxBox2dPolygon, public Stuff {
private:
    ofImage         texture;
    ofMesh          mesh;
    ofColor         color;

public:
    ofParameter<bool> useTexture;

    Asteroid()
        : useTexture(false)
    {
        // TODO? Allocate empty image?
    };

    void setup(b2World *world, float cx, float cy, float r=0) {
        if (r == 0)
            r = ofRandom(32, 64);
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
        int   nPts  = ofRandom(4, 12);
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

    void update() {
        /*
        // TODO: these need to be on the world, we need access.
        // So either give them access to world, tricky without a base we control.
        // Or do this in world and call methods for out of bounds, again the base is an issue.
        // Are we going to need to create our own hier so we don't have to wrap all the box2d classes?
        // Or don't inherit, wrap the ofxBox2dBaseShape instead.
        float left = 0 - border;
        float right = width + border;
        float top = 0 - border;
        float bottom = height + border;
        ofVec2f pos = getPosition();
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
        */
    };

    void draw() {
        ofPushStyle();
        ofPushMatrix();
        if (useTexture) {
            ofTranslate(getPosition());
            ofRotate(getRotation(), 0, 0, 1);
            drawTex();
        } else {
            ofNoFill();
            ofEnableAlphaBlending();
            ofSetLineWidth(7);
            ofSetColor(ofColor::blueSteel, 0.1);
            drawShape();
            ofSetLineWidth(3);
            ofSetColor(ofColor::white);
            drawShape();
            // Shadow
            ofSetLineWidth(0);
            ofFill();
            ofSetColor(0,0,0,100);
            drawShape();
        }
        ofPopMatrix();
        ofPopStyle();
    }

    // Draw shape in current style.
    void drawShape() {
        ofBeginShape();
        vector<ofPoint> pts = getPoints();
        for (int i=0; i<pts.size(); i++) {
            ofVertex(pts[i]);
        }
        ofEndShape(true);

    }

    void drawTex() {
        ofPushStyle();
        mesh.clearVertices();
        vector<ofPoint> &pts = getPoints();
        ofPoint center       = getCentroid2D();
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

    /*
    ofVec2f getPosition() {
        return polyShape.getPosition();
    };
    void setPosition(ofVec2f pos) {
        polyShape.setPosition(pos);
    };
    void setPosition(float x, float y) {
        polyShape.setPosition(x, y);
    };
    */
};

