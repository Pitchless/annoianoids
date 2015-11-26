
#pragma once

#include "ofMain.h"

/*
 * Class to handle the background, fills the window.
 */
class Background
{
protected:
    vector<ofImage> images_;
    int curImg_;
    string dataDirPath;

public:
    ofParameter<bool> showGradient, showImages;
    ofParameter<ofColor> color1;
    ofParameter<ofColor> color2;
    ofParameter<ofColor> colorTint;
    ofxPanel gui;

    Background()
      : curImg_(0)
      , showGradient("Show Gradient", false)
      , showImages("Show Images", true) {
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
      static ofxButton btnNext, btnPrev;
      gui.add(btnNext.setup("Next"));
      btnNext.addListener(this, &Background::next);
      gui.add(btnPrev.setup("Previous"));
      btnPrev.addListener(this, &Background::prev);
      gui.add(showGradient);
      gui.add(color1);
      gui.add(color2);
      gui.add(showImages);
      gui.add(colorTint);
    };

    void update() {};

    void draw() {
        ofPushStyle();
        if (showGradient)
            ofBackgroundGradient( color1, color2 );
        if (showImages && hasImages())
            drawImages();
	ofPopStyle();
    };

    void drawImages() {
        ofSetColor(colorTint);
        curImage().draw(0, 0, ofGetWidth(), ofGetHeight());
    };

    bool hasImages() {
        return images_.size() == 0 ? false : true;
    };

    ofImage& curImage() {
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
        sort(names.begin(), names.end());

        int num_loaded = 0;
        for (size_t i=0; i < names.size(); i++) {
            if ( addImage(names[i]) ) {
                num_loaded++;
            }
        }
        ofLogNotice() << "Loaded " << num_loaded << " image(s) in: " << dirname;
    };

    bool addImage(string fname) {
        ofImage img;
        if (!img.loadImage(fname)) {
            ofLogError() << "Failed to load texture file '" << fname << "'";
            return false;
        }
        images_.push_back(img);
        ofLogNotice() << "Loaded image file '" << fname << "'";
        return true;
    };
};
