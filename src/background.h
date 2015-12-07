
#pragma once

#include "ofMain.h"
#include "sprite_image.h"
#include "world.h"

/*
 * Class to handle the background, fills the window.
 */
class Background
{
protected:
    vector<SpriteImage> images_;
    int curImg_;
    string dataDirPath;
    int lastBg_s;

public:
    ofParameter<bool> showGradient, showImages;
    ofParameter<ofColor> color1;
    ofParameter<ofColor> color2;
    ofParameter<ofColor> colorTint;
    ofParameter<int> autoChangeSec;
    ofParameter<int> autoChangeMax;
    ofxPanel gui;
    World* world;
    

    Background()
      : curImg_(0)
      , showGradient("Show Gradient", false)
      , showImages("Show Images", true)
      , autoChangeSec("Auto Advance", 3, 0, 60)
      , autoChangeMax("Auto Max", 60, 1, 300)
      , lastBg_s(0)
      {
        color1.set( "BGColor1", ofColor ( 0,0,0,255 ),ofColor ( 0,0,0,0 ),ofColor ( 255,255,255,255 ) );
        color2.set( "BGColor2", ofColor ( 255,255,255,255 ),ofColor ( 0,0,0,0 ),ofColor ( 255,255,255,255 ) );
        colorTint.set( "Image Tint", ofColor ( 255,255,255,255 ),ofColor ( 0,0,0,0 ),ofColor ( 255,255,255,255 ) );
    };
    virtual ~Background() {};

    void setup(string dirname="") {
        dataDirPath = dirname;
        if (dirname != "")
            loadDir(dataDirPath);
	setupGui();
    };
    
    void setupGui() {
      gui.setup("Background");
      float w = gui.getWidth();
      static ofxButton btnNext, btnPrev;
      gui.add(btnNext.setup("Next Image", w, 32));
      btnNext.addListener(this, &Background::next);
      gui.add(btnPrev.setup("Previous", w, 32));
      btnPrev.addListener(this, &Background::prev);
      gui.add(showGradient);
      gui.add(showImages);
      gui.add(autoChangeSec);
      gui.add(autoChangeMax);
      gui.add(color1);
      gui.add(color2);
      gui.add(colorTint);
    };

    void update() {
      float now = ofGetElapsedTimef();
      if ( autoChangeSec > 0 && (now - lastBg_s > autoChangeSec)) {
        lastBg_s = now;
        next();
	autoChangeSec = ofRandom(2,autoChangeMax);
	world->clear();
      }
    };

    void draw() {
        ofPushStyle();
        if (showGradient) ofBackgroundGradient( color1, color2 );
        if (showImages && hasImages()) drawImages();
	ofPopStyle();
    };

    void drawImages() {
        ofSetColor(colorTint);
        curImage().draw(0, 0, ofGetWidth(), ofGetHeight());
    };

    bool hasImages() {
        return images_.size() == 0 ? false : true;
    };

    SpriteImage& curImage() {
        return images_[curImg_];
    };

    void next() {
        curImg_++;
        if (curImg_ > images_.size()-1)
            curImg_ = 0;
    }

    void prev() {
        curImg_--;
        if (curImg_ < 0)
            curImg_ = images_.size()-1;
    }

    void loadDir(string dirname) {
        ofLogNotice() << "Loading media from: " << dirname;
        ofDirectory dir(dirname);
        dir.listDir();
        if (dir.numFiles() == 0) {
            images_.resize(1); // at least one blank
            ofLogNotice() << "No images found, added default blank.";
            return;
        }
        vector<string> names;
        for (size_t i=0; i < dir.numFiles(); i++) {
            names.push_back(dir.getPath(i));
        }
        //sort(names.begin(), names.end());
        std::random_shuffle(names.begin(), names.end());

        int num_loaded = 0;
        for (size_t i=0; i < names.size(); i++) {
            if ( addImage(names[i]) ) {
                num_loaded++;
            }
        }
        ofLogNotice() << "Loaded " << num_loaded << " image(s) in: " << dirname;
    };

    bool addImage(string fname) {
        SpriteImage img;
        if (!img.load(fname)) {
            ofLogError() << "Failed to load texture file '" << fname << "'";
            return false;
        }
        images_.push_back(img);
        ofLogNotice() << "Loaded image file '" << fname << "'";
        return true;
    };
};
