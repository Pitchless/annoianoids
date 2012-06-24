#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// Custom controls.
#include "ofxFpsSlider.h"
#include "ofxGuiImage.h"

class kinectGuiApp : public ofBaseApp {

    public:
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

        void setupGui();
        ofxPanel guiApp;
        ofxFpsSlider fpsSlider;
        ofxToggle showGui;

        ofxPanel guiKinect;
        ofxFloatSlider kinectAngle;
        ofxGuiImage colorImageGui;
        ofxGuiImage depthImageGui;
        ofxGuiImage grayImageGui;

        void startKinect();
        void setKinectAngle( float & n_angle );
        void setNearThreshold( int n );
        void setFarThreshold( int n);
        void drawPointCloud();
        ofxKinect kinect;
        ofxCvColorImage colorImg;
        ofxCvGrayscaleImage depthImage;// grayscale depth image
        ofxCvGrayscaleImage grayImage; // grayscale depth image after threshold
        ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
        ofxCvGrayscaleImage grayThreshFar;  // the far thresholded image
        ofxCvContourFinder contourFinder;
        ofxToggle bThreshWithOpenCV;
        bool bDrawPointCloud;
        ofxIntSlider nearThreshold;
        ofxIntSlider farThreshold;
        ofxToggle kinectFlip;
        // used for viewing the point cloud
        ofEasyCam easyCam;
};
