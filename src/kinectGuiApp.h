#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// Custom controls.
#include "ofxFpsSlider.h"
#include "ofxGuiImage.h"

#include "KinectBlobTracker.h"

class kinectGuiApp : public ofBaseApp {

    public:
        KinectBlobTracker kinect;
        ofParameter<bool> showGui;
        ofParameter<bool> showPointCloud;
        ofParameter<bool> showColorImg;
        ofParameter<bool> showDepthImg;
        ofParameter<bool> showMaskImg;
        ofParameter<bool> showStencilImg;
        ofParameter<bool> showGrayImg;
        ofParameter<bool> showBlobs;
        ofParameter<bool> showVideo;
        ofParameter<ofColor> bgColor1;
        ofParameter<ofColor> bgColor2;

        void setup();
        void update();
        void draw();

        void keyPressed  (int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        void exit();

        vector<ofVideoPlayer> videos;
        ofVideoPlayer& getCurVideo();
        bool addVideo(string filename);
        void playVideo(size_t vid_numer=0);

        void setupGui();
        void loadSettings();
        void saveSettings();
        void grabMask();
        void clearMask();
        void drawKinectImages();
        void connect();

        ofxPanel guiApp;
        ofParameterGroup appParams;
        ofxFpsSlider fpsSlider;
        ofxButton loadButton;
        ofxButton saveButton;
        ofxLabel status;

        ofxPanel guiKinect;
        ofParameterGroup connectionParams;
        ofParameterGroup kinectParams;
        ofParameterGroup blobParams;
        ofxButton reConnectButton;
        ofxButton grabMaskButton;
        ofxButton clearMaskButton;
        string maskFilename;

        ofxPanel guiImages;
        ofxGuiImage colorImgGui;
        ofxGuiImage depthImgGui;
        ofxGuiImage maskImgGui;
        ofxGuiImage stencilImgGui;
        ofxGuiImage grayImgGui;

        // used for viewing the point cloud
        ofEasyCam easyCam;
};
