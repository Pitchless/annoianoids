#include "KinectBlobTracker.h"

KinectBlobTracker::KinectBlobTracker()
    : bInfrared("Grab Infrared", false)
    , bVideo("Grab Video", true)
    , bTexture("Use Texture", true)
    , bDepthRegistration("Depth Registration", false)
    , deviceId("ID", -1, -1,10)
    , serial("Serial", "")
    , nearClip("Near Depth Clipping (mm)", 500, 500, 4000)
    , farClip("Far Clipping (mm)", 4000, 500, 4000)
    , retryInCounter(-1)
    {
    boundingColor = ofColor::green;
    lineColor.set("Line Color", ofColor::yellow, ofColor(0,0), ofColor(255,255));
    kinectAngle.set("Angle", 0.0, -30.0, 30.0);
    bThresholds.set("Apply Threshold", false);
    nearThreshold.set("Near Threshold", 255, 0, 255);
    farThreshold.set("Far Threshold", 0, 0, 255);
    kinectFlip.set("H Flip", false);
    bMask.set("Apply Mask", false);
    extraMaskDepth.set("Extra Mask Depth", 0, 0, 100);
    medianBlur.set("Median Blur", 0, 0, 99);
    gaussianBlur.set("Gaussian Blur", 0, 0, 23);
    int kinectArea = kinect.width*kinect.height;
    minArea.set("Min Blob Area", 10, 0, (kinectArea/2) );
    maxArea.set("Max Blob Area", (kinectArea/2), 0, kinectArea );
    maxBlobs.set("Max Blobs", 10, 0, 200);
    bFindHoles.set("Find Holes", false);
    bUseApproximation.set("Use Approximation", false);
    simplify.set("Simplify Blob Lines", 0.0, 0.0, 10.0);
    resampleSpacing.set("Resample Spacing", 0.0, 0.0, kinect.width);
    resampleCount.set("Resample Count", 0, 0, 1000);

    showVerts.set("Show Verts", false);
    showInfo.set("Show Info", false);
    bFill.set("Fill", false);
    lineWidth.set("Line Width", 2.0, 0.0, 60.0);

    nearClip.addListener(this, &KinectBlobTracker::clipChange);
    farClip.addListener(this, &KinectBlobTracker::clipChange);
}

KinectBlobTracker::~KinectBlobTracker() {
    kinectAngle.removeListener(this, &KinectBlobTracker::setCameraTiltAngle);
    nearClip.removeListener(this, &KinectBlobTracker::clipChange);
    farClip.removeListener(this, &KinectBlobTracker::clipChange);
}

void KinectBlobTracker::clipChange(float & val) {
    kinect.setDepthClipping(nearClip, farClip);
}

void KinectBlobTracker::setup() {
    // Grab some memory to store out images.
    colorImg.allocate(kinect.width, kinect.height);
    depthImg.allocate(kinect.width, kinect.height);
    grayImg.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    maskImg.allocate(kinect.width, kinect.height);
    stencilImg.allocate(kinect.width, kinect.height);
    tempGrayImg.allocate(kinect.width, kinect.height);

    // Changing these settings requires a re-connect
    bDepthRegistration.addListener(this, &KinectBlobTracker::connectionSettingChange);
    bVideo.addListener(this, &KinectBlobTracker::connectionSettingChange);
    bInfrared.addListener(this, &KinectBlobTracker::connectionSettingChange);
    bTexture.addListener(this, &KinectBlobTracker::connectionSettingChange);

    // Move the camera when param changes.
    kinectAngle.addListener(this, &KinectBlobTracker::setCameraTiltAngle);
}

bool KinectBlobTracker::connectionSettingChange(bool& val) {
    if (kinect.isConnected()) {
        return reConnect();
    }
    return val;
}


bool KinectBlobTracker::connect() {
    // enable depth->video image calibration
    colorImg.set(0);
    kinect.setRegistration(bDepthRegistration);
    kinect.init(bInfrared, bVideo, bTexture);
    //kinect.open(1);   // open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");  // open a kinect using it's unique serial #
    bool result = kinect.open();      // opens first available kinect
    if (result) {
        ofLogNotice() << "Opened kinect";
        kinect.setCameraTiltAngle(kinectAngle);
        // Some time to settle the kinect.
        ofSleepMillis(1000);
    }
    else {
        ofLogError() << "Failed to open kinect";
    }
    deviceId = kinect.getDeviceId();
    serial = kinect.getSerial();
    return result;
}

bool KinectBlobTracker::reConnect() {
    ofLogNotice() << "Reconnecting";
    if (kinect.isConnected()) {
        close();
        ofSleepMillis(1000);
    }
    return connect();
}

void KinectBlobTracker::update() {
    if (!kinect.isConnected()) {
        if (retryInCounter < 0) {
            ofLogWarning() << "Kinect not found, re-connect in 2...";
            retryInCounter = 120;
        }
        if (--retryInCounter == 0) {
            connect();
        }
        return;
    }
    retryInCounter = -1;
    kinect.update();
    if(!kinect.isFrameNew()) return;

    // load the rgb image
    if (bVideo) {
        colorImg.setFromPixels(kinect.getPixels());
        if (kinectFlip)
            colorImg.mirror(false,true);
    }

    // load grayscale depth image from the kinect source
    depthImg.setFromPixels(kinect.getDepthPixels());
    if (kinectFlip)
        depthImg.mirror(false,true);

    grayImg = depthImg;

    // Apply the thresholds, setting pix to black when past thresh.
    if (bThresholds) {
        cvThreshold(grayImg.getCvImage(), grayImg.getCvImage(), farThreshold, 0, CV_THRESH_TOZERO);
        cvThreshold(grayImg.getCvImage(), grayImg.getCvImage(), nearThreshold, 0, CV_THRESH_TOZERO_INV);
    }

    if (bMask) {
        // Make a stencil. fooImg is 255 for keep and 0 for remove.
        cvCmp(depthImg.getCvImage(), maskImg.getCvImage(), stencilImg.getCvImage(), CV_CMP_GT);
        // Apply the stencil to the depth image.
        cvAnd(grayImg.getCvImage(), stencilImg.getCvImage(), grayImg.getCvImage());
        stencilImg.flagImageChanged();
    }

    if (medianBlur > 0) {
        if (medianBlur % 2 == 0) medianBlur++; // must be odd
        tempGrayImg = grayImg;
        cvSmooth(tempGrayImg.getCvImage(), grayImg.getCvImage(), CV_MEDIAN, medianBlur);
    }

    if (gaussianBlur > 0) {
        if (gaussianBlur % 2 == 0) gaussianBlur++; // must be odd
        grayImg.blurGaussian(gaussianBlur);
    }

    // update the cv images
    grayImg.flagImageChanged();

    findBlobs();
}

void KinectBlobTracker::findBlobs() {
    contourFinder.findContours(grayImg, minArea, maxArea, maxBlobs, bFindHoles, bUseApproximation);
    if (contourFinder.blobs.size() == 0) return;

    blobs.clear();
    for (size_t i=0; i < contourFinder.blobs.size(); ++i) {
        ofPolyline nline;
        nline.addVertices(contourFinder.blobs[i].pts);
        nline.setClosed(true);
        if (simplify > 0.0) {
            nline.simplify(simplify);
        }
        if (resampleSpacing > 0.0) {
            tmpLine = nline.getResampledBySpacing(resampleSpacing);
            nline = tmpLine;
        }
        if (resampleCount > 0) {
            tmpLine = nline.getResampledByCount(resampleCount);
            nline = tmpLine;
        }
        blobs.push_back(nline);
    }
}

void KinectBlobTracker::setCameraTiltAngle(float & n_angle) {
    //if (n_angle>30)  n_angle=30;
    //if (n_angle<-30) n_angle=-30;
    kinect.setCameraTiltAngle(n_angle);
}

void KinectBlobTracker::grabMask() {
    maskImg = depthImg;

    // Add a bit to each value to pull the mask forward a bit, help deal with
    // noise in the kinect data.
    // TODO - Do this with cvAdd, could be faster?
    ofPixels pix = maskImg.getPixels();
    for (size_t i = 0; i < pix.size(); ++i) {
        pix[i] = ofClamp(pix[i]+extraMaskDepth, 0, 255);
    }
}

void KinectBlobTracker::clearMask() {
    maskImg.set(0);
}

bool KinectBlobTracker::loadMask(string filename) {
    ofFile maskfile(filename);
    if (!maskfile.exists())
        return false;

    ofImage loadImg;
    if (!loadImg.load(filename))
        return false;
    ofxCvColorImage cvColorImg;
    cvColorImg.allocate(maskImg.width, maskImg.height);
    cvColorImg.setFromPixels(loadImg.getPixels());
    maskImg = cvColorImg;
    return true;
}

void KinectBlobTracker::saveMask(string filename) {
    // Save the mask. CV images have no save so we convert to ofImage
    // Can't seem to do that for a cvGrayScale image so convert to color first.
    ofxCvColorImage cvColorImg;
    cvColorImg.allocate(maskImg.width, maskImg.height);
    cvColorImg = maskImg;
    ofImage saveImg;
    //saveImg.setFromPixels(cvColorImg.getPixels(), cvColorImg.width, cvColorImg.height, OF_IMAGE_COLOR);
    saveImg.setFromPixels(cvColorImg.getPixels());
    saveImg.save(filename);
}

void KinectBlobTracker::drawBlobs(float x, float y, float w, float h) {
    float scalex = w/kinect.width;
    float scaley = h/kinect.height;
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(x,y,0.0);
    ofScale(scalex, scaley, 0.0);

        ofNoFill();
        // Draw the blobs
        vector<ofPolyline>::iterator it;
        for (it = blobs.begin(); it != blobs.end(); ++it) {
            // Draw the line
            ofSetColor(lineColor);
            ofSetLineWidth(lineWidth);
            if (bFill) ofFill();
            else ofNoFill();
//            it->draw();
            ofBeginShape();
                vector<ofPoint> pts = it->getVertices();
                for (size_t i=0; i<pts.size(); ++i) {
                    ofVertex(pts[i].x, pts[i].y);
                }
           ofEndShape(true);

            // Verts
            if (showVerts) {
                vector<ofPoint> pts = it->getVertices();
                for (size_t j=0; j<pts.size(); ++j) {
                    ofSetColor(255,0,255,100);
                    ofFill();
                    ofDrawCircle(pts[j].x, pts[j].y, 10);
                    ofSetColor(0,0,0);
                    ofDrawBitmapString(ofToString(j), pts[j].x-6, pts[j].y+3);
                    ofSetColor(255,255,255);
                    ofNoFill();
                    ofDrawCircle(pts[j].x, pts[j].y, 10);
                }
            }

            // Bounding and info
            if (showInfo) {
                stringstream info;
                info << "Size:" << it->size()
                     << " Area:" << it->getArea()
                     << " Perimeter:" << it->getPerimeter();
                ofSetColor(boundingColor);
                ofRectangle box = it->getBoundingBox();
                ofDrawRectangle(box);
                ofDrawBitmapString(info.str(), box.x, box.y);
            }
        }

    ofPopMatrix();
    ofPopStyle();
}

void KinectBlobTracker::drawPointCloud() {
    int w = 640;
    int h = 480;
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    int step = 2;
    for(int y = 0; y < h; y += step) {
        for(int x = 0; x < w; x += step) {
            if(kinect.getDistanceAt(x, y) > 0) {
                mesh.addColor(kinect.getColorAt(x,y));
                mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
            }
        }
    }
    glPointSize(3);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards'
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    glEnable(GL_DEPTH_TEST);
    mesh.drawVertices();
    glDisable(GL_DEPTH_TEST);
    ofPopMatrix();
}

void KinectBlobTracker::close() {
    ofLogNotice() << "Closing Kinect connection";
    kinect.close();
}
