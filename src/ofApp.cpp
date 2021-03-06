#include "ofApp.h"
#include "photon.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowTitle("Annoianoids");
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetWindowShape(1280, 960);

    maskFilename = "mask.png";

    // Video
    iCurVideo = 0;
    showVideo.set("Show Video", false);
    loadVideoDir("video");
    //playVideo();
    //pauseVideo();

    showMain.set("Show Main", true);
    showWorld.set("Show World", true);
    mainAlpha.set("Main Alpha", 223, 0, 255);
    mainHue.set("Main Hue", 255, 0, 255);
    mainSaturation.set("Main Saturation", 255, 0, 255);
    imgMain.allocate(kinect.kinect.width, kinect.kinect.height, OF_IMAGE_COLOR_ALPHA);
    mainRotation.set("Rotation", 0, 0, 360);
    scale.set("Scale", 1.0, 0.1, 10.0);
    pointMode.set("Point Mode",6,0,6);
    bPointColor.set("Point Color", true);

    bg.setup("bg");

    kinect.setup();
    setupGui();
    loadSettings();
    kinect.connect();

    cueVideo(0);

    world.setup(VIEW_W, VIEW_H);
    world.gravityY = 2.3;
}

//--------------------------------------------------------------

void ofApp::loadVideoDir(string dirname) {
    ofLogNotice() << "Loading video from: " << dirname;
    iCurVideo = 0;
    ofDirectory dir(dirname);
    dir.listDir();
    if (dir.size() == 0) {
        videos.resize(1); // at least one blank video
        ofLogNotice() << "No video found, added default blank video.";
        return;
    }
    vector<string> names;
    for (size_t i=0; i < dir.size(); i++) {
        names.push_back(dir.getPath(i));
    }
    sort(names.begin(), names.end());

    int num_loaded = 0;
    for (size_t i=0; i < names.size(); i++) {
        if ( addVideo(names[i]) ) {
            num_loaded++;
        }
    }
    ofLogNotice() << "Loaded " << num_loaded << " video(s) in: " << dirname;
}

ofVideoPlayer& ofApp::getCurVideo() {
    return videos[iCurVideo];
}

bool ofApp::addVideo(string filename) {
    ofVideoPlayer vid;
    ofLogNotice() << "Loading movie: " << filename;
    //videoPlayer.setPixelFormat(OF_PIXELS_RGBA);
    if (!vid.load(filename)) {
        ofLogError() << "Failed to load movie: " << filename;
        return false;
    }
    vid.setLoopState(OF_LOOP_NONE);
    videos.push_back(vid);
    return true;
}

void ofApp::playVideo() {
    getCurVideo().play();
    ofLogNotice() << "Playing video: " << getCurVideo().getMoviePath();
}

void ofApp::togglePlayVideo() {
    ofVideoPlayer vid = getCurVideo();
    if ( vid.isPaused() ) {
        playVideo();
    }
    else {
        pauseVideo();
    }
}

void ofApp::pauseVideo() {
    getCurVideo().setPaused(true);
    ofLogNotice() << "Pause video: " << getCurVideo().getMoviePath();
}

void ofApp::cueNextVideo() {
    int num = iCurVideo + 1;
    if ( num > videos.size()-1 ) {
        num = 0;
    }
    cueVideo(num);
}

void ofApp::cueVideo(int num) {
    return;
    if ( num < 0 || num > videos.size()-1 ) {
        ofLogWarning() << "Attempt to cue unknown video: " << num;
        return;
    }
    getCurVideo().stop();
    iCurVideo = num;
    videos[iCurVideo].play();
    videos[iCurVideo].setPaused(true);
    videos[iCurVideo].firstFrame();
    ofLogNotice() << "Cue video: " << getCurVideo().getMoviePath();
}

void ofApp::playNextVideo() {
    cueNextVideo();
    playVideo();
}

void ofApp::setupGui() {
    ofxGuiSetHeaderColor( ofColor(100) );    // param group headers
    ofxGuiSetBackgroundColor( ofColor(60) ); // bg of params (sliders and stuff but not buttons)
    ofxGuiSetBorderColor( ofColor(200) );     // bg of param groups (but not panels)
    ofxGuiSetFillColor( ofColor(175,145,0) ); // Fill on slider bars etc
    ofxGuiSetTextColor( ofColor::white );
    ofxGuiSetFont("verdana.ttf", 10);
    //ofxGuiSetFont("Hyperspace Bold.otf", 10);

    // Note: The panels will get repositioned in windowResized
    guiApp.setup("KinectGui");
    guiApp.add( fpsSlider.setup("FPS", 60) );
    guiApp.add( reConnectButton.setup("Connect") );
    guiApp.add( loadButton.setup("Load") );
    guiApp.add( saveButton.setup("Save") );
    guiApp.add( grabMaskButton.setup("Grab Mask") );
    guiApp.add( clearMaskButton.setup("Clear Mask") );
    //guiApp.add( playVideoButton.setup("Play Video") );
    //guiApp.add( pauseVideoButton.setup("Pause Video") );
    //guiApp.add( cueNextVideoButton.setup("Cue Next Video") );
    //guiApp.add( nextVideoButton.setup("Play Next Video") );
    appParams.setName("Display");
    appParams.add( showGui.set("Show Gui", true) );
    appParams.add( showPointCloud.set("Show Point Cloud", false) );
    appParams.add( pointMode );
    appParams.add( bPointColor );
    appParams.add( showColorImg.set("RGB", false) );
    appParams.add( showDepthImg.set("Depth", false) );
    appParams.add( showMaskImg.set("Mask", false) );
    appParams.add( showStencilImg.set("Stencil", false) );
    appParams.add( showGrayImg.set("Gray", false) );
    appParams.add( showBlobs.set("Show Blobs", true) );
    appParams.add( showWorld );
    appParams.add( autoAddRate.set("Auto Add Rate", 0, 0, 10) );
    appParams.add( autoAddMinBlobs.set("Auto Add Min Blobs", 0, 0, 10) );
    appParams.add( autoHueRate.set("Auto Hue Rate", 3, 1, 20) );
    appParams.add( autoOutline.set("Auto Outline", 0, 0, 20) );
    //appParams.add( showVideo );
    appParams.add( mainRotation );
    appParams.add( scale );
    appParams.add( showMain );
    appParams.add( mainAlpha );
    appParams.add( mainHue );
    appParams.add( mainSaturation );
    guiApp.add( appParams );
    guiApp.add( status.setup("Status","") );
    reConnectButton.addListener(this, &ofApp::connect);
    loadButton.addListener(this, &ofApp::loadSettings);
    saveButton.addListener(this, &ofApp::saveSettings);
    grabMaskButton.addListener(this, &ofApp::grabMask);
    clearMaskButton.addListener(this, &ofApp::clearMask);
    playVideoButton.addListener(this, &ofApp::playVideo);
    pauseVideoButton.addListener(this, &ofApp::pauseVideo);
    cueNextVideoButton.addListener(this, &ofApp::cueNextVideo);
    nextVideoButton.addListener(this, &ofApp::playNextVideo);

    guiKinect.setup("Kinect");
    ofxGuiGroup * guiKinectGroup = new ofxGuiGroup();
    guiKinectGroup->setup("");
    // Open settings
    connectionParams.setName("Connection");
    connectionParams.add( kinect.deviceId );
    connectionParams.add( kinect.serial );
    connectionParams.add( kinect.bDepthRegistration );
    connectionParams.add( kinect.bVideo );
    connectionParams.add( kinect.bInfrared );
    connectionParams.add( kinect.bTexture );
    guiKinectGroup->add( connectionParams );

    // Run time settings
    kinectParams.setName("Settings");
    kinectParams.add( kinect.kinectAngle );
    kinectParams.add( kinect.kinectFlip );
    kinectParams.add( kinect.nearThreshold );
    kinectParams.add( kinect.farThreshold );
    kinectParams.add( kinect.bThresholds );
    kinectParams.add( kinect.nearClip );
    kinectParams.add( kinect.farClip );
    kinectParams.add( kinect.extraMaskDepth );
    kinectParams.add( kinect.bMask );
    guiKinectGroup->add( kinectParams );

    blobParams.setName("Blobs");
    blobParams.add( kinect.medianBlur );
    blobParams.add( kinect.gaussianBlur );
    blobParams.add( kinect.minArea );
    blobParams.add( kinect.maxArea );
    blobParams.add( kinect.maxBlobs );
    blobParams.add( kinect.bFindHoles );
    blobParams.add( kinect.bUseApproximation );
    blobParams.add( kinect.simplify );
    blobParams.add( kinect.resampleSpacing );
    blobParams.add( kinect.resampleCount );
    blobParams.add( kinect.showInfo );
    blobParams.add( kinect.showVerts );
    blobParams.add( kinect.lineWidth );
    blobParams.add( kinect.lineColor );
    blobParams.add( kinect.bFill );
    guiKinectGroup->add(blobParams);

    guiKinect.add( guiKinectGroup );

    // Images
    // Hide the names and use toggles as labels on the images.
    guiImages.setup("Images");
    //guiImages.setSize(100,100);
    ofxGuiGroup * guiImgGroup = new ofxGuiGroup();
    guiImgGroup->setup("");
    //guiImgGroup->setSize(100,18);
    int imgWidth = 200;
    guiImgGroup->add( colorImgGui.setup("Color", (ofImage*)&kinect.colorImg, true, imgWidth) );
    guiImgGroup->add( depthImgGui.setup("Depth", (ofImage*)&kinect.depthImg, true) );
    guiImgGroup->add( maskImgGui.setup("Mask", (ofImage*)&kinect.maskImg, true) );
    guiImgGroup->add( stencilImgGui.setup("Stencil", (ofImage*)&kinect.stencilImg, true) );
    guiImgGroup->add( grayImgGui.setup("Gray", (ofImage*)&kinect.grayImg, true) );
    guiImages.add( guiImgGroup );

    world.setupGui();
}

void ofApp::connect() {
    kinect.reConnect();
}

//--------------------------------------------------------------
void ofApp::loadSettings() {
    guiApp.loadFromFile("settings.xml");
    guiKinect.loadFromFile("kinect.xml");
    bg.gui.loadFromFile("bg.xml");
    world.gui.loadFromFile("world.xml");
    kinect.loadMask(maskFilename);
}

void ofApp::saveSettings() {
    guiApp.saveToFile("settings.xml");
    guiKinect.saveToFile("kinect.xml");
    bg.gui.saveToFile("bg.xml");
    world.gui.saveToFile("world.xml");
    kinect.saveMask(maskFilename);
}

void ofApp::grabMask() {
    kinect.grabMask();
}

void ofApp::clearMask() {
    kinect.clearMask();
    ofFile maskfile(maskFilename);
    if (maskfile.exists())
        maskfile.remove();
}

//--------------------------------------------------------------
void ofApp::update() {
    bg.update();
    getCurVideo().update();
    kinect.update();
    float scalex = VIEW_W/kinect.kinect.width;
    float scaley = VIEW_H/kinect.kinect.height;
    vector<ofPolyline> bloblines;
    for (int i=0; i<kinect.blobs.size(); i++ ) {
        ofPolyline ln;
        vector<ofPoint> verts = kinect.blobs[i].getVertices();
        for (int j=0; j<verts.size(); j++) {
            ln.addVertex(verts[j].x *scalex, verts[j].y*scaley);
        }
        bloblines.push_back(ln);
    }
    world.updateOutlines(bloblines);
    size_t numBlobs = kinect.blobs.size();
    if (autoAddRate > 0 && (int)numBlobs >= autoAddMinBlobs) {
        static int last = 0;
        int elapsed = int(ofGetElapsedTimef());
        if ((elapsed % autoAddRate) == 0 && last != elapsed) {
            last = elapsed;
            addShiz();
        }
    }
    if (autoHueRate > 0) {
        static int last = 0;
        int elapsed = int(ofGetElapsedTimef());
        if ((elapsed % autoHueRate) == 0 && last != elapsed) {
            last = elapsed;
            mainHue += 1;
        }
    }
    if (autoOutline > 0) {
        static int last = 0;
        int elapsed = int(ofGetElapsedTimef());
        if ((elapsed % autoOutline) == 0 && last != elapsed) {
            int roll = ofRandom(1,3);
            last = elapsed;
            if (roll==1) {
                showBlobs = true;
                showMain = false;
            }
            if (roll==2) {
                showBlobs = false;
                showMain = true;
            }
            if (roll==3) {
                showBlobs = true;
                showMain = true;
            }
        }
    }

    world.update();

    // Copy the kinect grey image into our video layer
    ofPixels newPix = imgMain.getPixels();
    ofPixels pix = kinect.grayImg.getPixels();
    ofColor col(0,0,0,0);
    ofColor blank(0,0,0,0);
    for ( int i=0; i<pix.size(); i++ ) {
        if ( pix[i] < 1 ) {
            newPix[i*4]   = 0;
            newPix[i*4+1] = 0;
            newPix[i*4+2] = 0;
            newPix[i*4+3] = 0;
        }
        else {
            int val = pix[i];
            ofColor newcol = ofColor::fromHsb(mainHue, mainSaturation, pix[i], mainAlpha);
            newPix[i*4]   = newcol.r;
            newPix[i*4+1] = newcol.g;
            newPix[i*4+2] = newcol.b;
            newPix[i*4+3] = newcol[3];
        }
    }
    imgMain.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    float w = VIEW_W;
    float h = VIEW_H;

    bg.draw();
    drawKinectImages();

    ofPushStyle();
    ofPushMatrix();
    ofTranslate((w/2.0), (h/2.0));
    ofRotate(mainRotation);
    ofTranslate(-(w/2.0), -(h/2.0));
    ofScale(scale, scale, 0.0);
    //if (showVideo)
    //    getCurVideo().draw(0,0,w,h);
    if (showPointCloud) {
        easyCam.begin();
        //kinect.drawPointCloud();
        drawPointCloud();
        easyCam.end();
    }
    if (showWorld) world.draw();
    if (showMain)  imgMain.draw(0,0,w,h);
    if (showBlobs) kinect.drawBlobs(0,0,w,h);
    ofPopMatrix();
    ofPopStyle();

    if (showGui) {
        bg.gui.draw();
        guiApp.draw();
        guiKinect.draw();
        guiImages.draw();
        world.gui.draw();
    }
}

void ofApp::drawKinectImages() {
    vector<ofxCvImage*> images;
    if (showColorImg)   images.push_back(&kinect.colorImg);
    if (showDepthImg)   images.push_back(&kinect.depthImg);
    if (showMaskImg)    images.push_back(&kinect.maskImg);
    if (showStencilImg) images.push_back(&kinect.stencilImg);
    if (showGrayImg)    images.push_back(&kinect.grayImg);
    int numImg = images.size();

    int ww = ofGetWidth();
    int wh = ofGetHeight();
    ofRectangle rects[4];
    if (numImg == 1) {
        rects[0] = ofRectangle(0,0,ww,wh);
    }
    else if (numImg < 5) { // 2 - 4
        int w = ww/2;
        int h = wh/2;
        rects[0] = ofRectangle(0,0,w,h);
        rects[1] = ofRectangle(w,0,w,h);
        rects[2] = ofRectangle(0,h,w,h);
        rects[3] = ofRectangle(w,h,w,h);
    }
    else {
        // 4 max!
        numImg = 4;
    }

    for (int i = 0; i < numImg; ++i)
        images[i]->draw(rects[i]);
}

void ofApp::drawPointCloud() {
    int w = 640;
    int h = 480;
    ofMesh mesh;
    //mesh.setMode(OF_PRIMITIVE_POINTS);
    //mesh.setMode(OF_PRIMITIVE_TRIANGLES); // needs indices
    //mesh.setMode(OF_PRIMITIVE_LINES); // excellent crazy lines
    //mesh.setMode(OF_PRIMITIVE_LINE_STRIP); // wicked crazy lines
    //mesh.setMode(OF_PRIMITIVE_LINE_LOOP); // wicked crazy lines
    mesh.setMode(static_cast<ofPrimitiveMode>(pointMode.get()));
    int step = 2;
    for(int y = 0; y < h; y += step) {
        for(int x = 0; x < w; x += step) {
            if(kinect.kinect.getDistanceAt(x, y) > 0) {
                if (bPointColor) {
                    mesh.addColor(kinect.kinect.getColorAt(x,y));
                }
                else {
                    mesh.addColor(kinect.lineColor.get());
                }
                mesh.addVertex(kinect.kinect.getWorldCoordinateAt(x, y));
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

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == '\t') {
        showGui = !showGui;
    }
    if (key == 'F') {
        ofToggleFullscreen();
    }
    if (key == 'S') {
        saveSettings();
    }
    if (key == 'L') {
        loadSettings();
    }
    if (key == 'g') {
        grabMask();
    }

    if(key == 'C') {
        world.clear();
    }
    if(key == 'c') {
        world.addCircle(mouseX/scale, mouseY/scale);
    }
    if(key == 'b') {
        world.addRect(mouseX/scale, mouseY/scale);
    }
    if(key == 'a') {
        world.addAsteroid(mouseX/scale, mouseY/scale);
    }
    if(key == 'n') {
        addShiz();
    }
    if (key == '1') addShiz(1);
    if (key == '2') addShiz(2);
    if (key == '3') addShiz(3);
    if (key == '4') addShiz(4);
    if (key == '5') addShiz(5);
    if (key == '6') addShiz(6);
    if (key == '7') addShiz(7);
    if (key == '8') addShiz(8);
    if (key == '9') addShiz(9);
    if(key == 'p') {
        PhotonPtr p = PhotonPtr(new Photon);
        p->setup(world.getB2World(), mouseX/scale, mouseY/scale);
        world.add((StuffPtr)p);
        p->setVelocity(ofRandom(-10, 10), ofRandom(-10, 10));
    }
}

void ofApp::addShiz() {
    addShiz(int(ofRandom(1,11)));
};

void ofApp::addShiz(int shiz) {
    float x = ofRandom(0.0, world.width);
    float y = -50;
    ofLogNotice() << "SHIZ " << shiz << " " << x << "x" << y;
    if(shiz == 1) {
        world.addSprite(x, y, "smiley_yell.png", 23);
    }
    if(shiz == 2) {
        world.addSprite(x, y, "smiley_pink.png", 42, 1, 1, 1.4);
    }
    if(shiz == 3) {
        world.addSprite(x, y, "axe.png", 42, 2.2, 2.0, 0.02, 0.6);
    }
    if(shiz == 4) {
        world.addBoxSprite(x, y, "raggy1.png", 6, 3.0, 1.0, 0.1, 1.6);
    }
    if(shiz == 5) {
        for (int i=0; i<int(ofRandom(1,3)); i++) {
            float s = ofRandom(3,6);
            world.addBoxSprite(x, y, "dildo.gif", s, 0.9, 1.0, 0.5, 1.6);
        }
    }
    if(shiz == 6) {
        world.addSprite(x, y, "fanny.png", 40, 1.0, 1.0, 1.4, 0.5);
    }
    if(shiz == 7) {
        world.addSprite(x, y, "dino.png", 20, 2.0, 3.0, 0.4, 1.5);
    }
    if(shiz == 8) {
        world.addBoxSprite(x, y, "spider5.png", 5, 2.0, 1.0, 1.4, 0.5);
    }
    if(shiz == 9) {
        for (int i=0; i<int(ofRandom(1,3)); i++) {
            float s = ofRandom(20,30);
            world.addSprite(x, y, "raggy2.png", s, 2.0, 1.0, 1.4, 0.5);
        }
    }
};

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    float sw = ofGetWidth()/VIEW_W;
    float sh = ofGetHeight()/VIEW_H;
    scale = sw > sh ? sh : sw;
    ofLogNotice() << "Window: " << w << " x " << h << "Scale " << scale;

    // Re-layout the gui
    status = ofToString(w) + "x" + ofToString(h);
    bg.gui.setPosition(10,10);
    world.gui.setPosition(bg.gui.getShape().getRight()+10,10);
    guiImages.setPosition(world.gui.getShape().getRight()+10,10);
    guiKinect.setPosition(guiImages.getShape().getRight()+10,10);
    guiApp.setPosition(ofGetWidth()-guiApp.getShape().width-10, 10);
    bg.gui.minimizeAll();
    guiImages.minimizeAll();
    guiKinect.minimizeAll();
    guiBox2d.minimizeAll();

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

//--------------------------------------------------------------
void ofApp::exit() {
//    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();

    loadButton.removeListener(this, &ofApp::loadSettings);
    saveButton.removeListener(this, &ofApp::saveSettings);
    grabMaskButton.removeListener(this, &ofApp::grabMask);
}



