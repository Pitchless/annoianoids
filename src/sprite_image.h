#pragma once

#include "ofMain.h"
#include "ofxGif.h"

class SpriteImage : public ofImage {
private:
    size_t idx;
    bool isGif;
    ofxGIF::fiGifLoader gifloader;

public:
    ofParameter<int> frame_ms;
    SpriteImage() : idx(0), isGif(false), frame_ms("Frame Time ms", 60000, 2, 60000) {};
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
	  if (ofGetElapsedTimeMillis() % (uint64_t)frame_ms) {
	      idx++;
	      if (idx > gifloader.pages.size()-1) idx = 0;
	  }
          gifloader.pages[idx].draw(x, y, w, h);
      } else {
          ofImage::draw(x, y, w, h);
      }
    };
};
