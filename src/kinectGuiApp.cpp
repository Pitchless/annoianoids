#include "kinectGuiApp.h"

//--------------------------------------------------------------
void kinectGuiApp::setup(){
    ofSetWindowTitle("kinectGui");
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    maskFilename = "mask.png";
    bgColor1.set("bgColor1", ofColor(200,200,200),ofColor(0,0),ofColor(255,255));
    bgColor2.set("bgColor2", ofColor(23,23,23),ofColor(0,0),ofColor(255,255));

    // Video
    iCurVideo = 0;
    showVideo.set("Show Video", true);
    loadVideoDir("video");
    playVideo();

    // Midi
    // print ports to console
	midiIn.listPorts(); // via instance
	//ofxMidiIn::listPorts(); // via static as well

	// open port by number (you may need to change this)
	midiIn.openPort(1);
	//midiIn.openPort("IAC Pure Data In");	// by name
	//midiIn.openVirtualPort("ofxMidiIn Input");	// open a virtual port

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);

	// add app as a listener
	midiIn.addListener(this);

	// print received messages to the console
	midiIn.setVerbose(true);

    kinect.setup();
    setupGui();
    loadSettings();
    kinect.connect();
}

//--------------------------------------------------------------
void kinectGuiApp::newMidiMessage(ofxMidiMessage& msg) {
	// make a copy of the latest message
	midiMessage = msg;

    stringstream text;
    // draw the last recieved message contents to the screen
	text << "Received: " << ofxMidiMessage::getStatusString(midiMessage.status);
	text << " channel: " << midiMessage.channel;
	text << " pitch: " << midiMessage.pitch;
	text << " velocity: " << midiMessage.velocity;
	text << " control: " << midiMessage.control;
	text << " value: " << midiMessage.value;

//	if(midiMessage.status == MIDI_PITCH_BEND) {
//		ofRect(20, 202, ofMap(midiMessage.value, 0, MIDI_MAX_BEND, 0, ofGetWidth()-40), 20);
//	}
//	else {
//		ofRect(20, 202, ofMap(midiMessage.value, 0, 127, 0, ofGetWidth()-40), 20);
//	}

	text << "delta: " << midiMessage.deltatime;
    ofLogNotice() << "MIDI: " << text.str();

    if (msg.channel == 10 && msg.velocity > 0) {
        togglePlayVideo();
    }
}

void kinectGuiApp::loadVideoDir(string dirname) {
    ofLogNotice() << "Loading video from: " << dirname;
    iCurVideo = 0;
    ofDirectory dir(dirname);
    dir.listDir();
    if (dir.numFiles() == 0) {
        videos.resize(1); // at least one blank video
        ofLogNotice() << "No video found, added default blank video.";
        return;
    }
    //videos.resize(dir.numFiles());
    int num_loaded = 0;
    for (size_t i=0; i < dir.numFiles(); i++) {
        if ( addVideo(dir.getPath(i)) ) { num_loaded++; }
    }
    ofLogNotice() << "Loaded " << num_loaded << " video(s) in: " << dirname;
}

ofVideoPlayer& kinectGuiApp::getCurVideo(){
    return videos[iCurVideo];
}

bool kinectGuiApp::addVideo(string filename) {
    ofVideoPlayer vid;
    ofLogNotice() << "Loading movie: " << filename;
    //videoPlayer.setPixelFormat(OF_PIXELS_RGBA);
    if (!vid.loadMovie(filename)) {
        ofLogError() << "Failed to load movie: " << filename;
        return false;
    }
    vid.setLoopState(OF_LOOP_NORMAL);
    videos.push_back(vid);
    return true;
}

void kinectGuiApp::playVideo() { getCurVideo().play(); }

void kinectGuiApp::togglePlayVideo() {
    ofVideoPlayer vid = getCurVideo();
    if ( vid.isPaused() ) { vid.play(); }
    else { vid.setPaused(true); }
}

void kinectGuiApp::pauseVideo() { getCurVideo().setPaused(true);}

void kinectGuiApp::cueNextVideo() {
    getCurVideo().stop();
    iCurVideo++;
    if ( iCurVideo > videos.size()-1 ) { iCurVideo = 0; }
    videos[iCurVideo].play();
    videos[iCurVideo].setPaused(true);
    videos[iCurVideo].firstFrame();
}

void kinectGuiApp::playNextVideo(){
    cueNextVideo();
    playVideo();
}

void kinectGuiApp::setupGui() {
    ofxGuiSetHeaderColor( ofColor(100) );    // param group headers
    ofxGuiSetBackgroundColor( ofColor(60) ); // bg of params (sliders and stuff but not buttons)
    ofxGuiSetBorderColor( ofColor(200) );     // bg of param groups (but not panels)
    ofxGuiSetFillColor( ofColor(175,145,0) ); // Fill on slider bars etc
    ofxGuiSetTextColor( ofColor::white );
    ofxGuiSetFont("verdana.ttf", 10);

    // Note: The panels will get repositioned in windowResized
    guiApp.setup("KinectGui");
    guiApp.add( fpsSlider.setup("FPS", 60) );
    guiApp.add( reConnectButton.setup("Connect") );
    guiApp.add( loadButton.setup("Load") );
    guiApp.add( saveButton.setup("Save") );
    guiApp.add( grabMaskButton.setup("Grab Mask") );
    guiApp.add( clearMaskButton.setup("Clear Mask") );
    guiApp.add( playVideoButton.setup("Play Video") );
    guiApp.add( pauseVideoButton.setup("Pause Video") );
    guiApp.add( cueNextVideoButton.setup("Cue Next Video") );
    guiApp.add( nextVideoButton.setup("Play Next Video") );
    appParams.setName("Display");
    appParams.add( showGui.set("Show Gui", true) );
    appParams.add( showPointCloud.set("Show Point Cloud", true) );
    appParams.add( showColorImg.set("RGB", false) );
    appParams.add( showDepthImg.set("Depth", false) );
    appParams.add( showMaskImg.set("Mask", false) );
    appParams.add( showStencilImg.set("Stencil", false) );
    appParams.add( showGrayImg.set("Gray", false) );
    appParams.add( showBlobs.set("Show Blobs", false) );
    appParams.add( showVideo );
    appParams.add( bgColor1 );
    appParams.add( bgColor2 );
    guiApp.add( appParams );
    guiApp.add( status.setup("Status","") );
    reConnectButton.addListener(this, &kinectGuiApp::connect);
    loadButton.addListener(this, &kinectGuiApp::loadSettings);
    saveButton.addListener(this, &kinectGuiApp::saveSettings);
    grabMaskButton.addListener(this, &kinectGuiApp::grabMask);
    clearMaskButton.addListener(this, &kinectGuiApp::clearMask);
    playVideoButton.addListener(this, &kinectGuiApp::playVideo);
    pauseVideoButton.addListener(this, &kinectGuiApp::pauseVideo);
    cueNextVideoButton.addListener(this, &kinectGuiApp::cueNextVideo);
    nextVideoButton.addListener(this, &kinectGuiApp::playNextVideo);

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
}

void kinectGuiApp::connect() { kinect.reConnect(); }

//--------------------------------------------------------------
void kinectGuiApp::loadSettings() {
    guiApp.loadFromFile("settings.xml");
    guiKinect.loadFromFile("kinect.xml");
    kinect.loadMask(maskFilename);
}

void kinectGuiApp::saveSettings() {
    guiApp.saveToFile("settings.xml");
    guiKinect.saveToFile("kinect.xml");
    kinect.saveMask(maskFilename);
}

void kinectGuiApp::grabMask() {
    kinect.grabMask();
}

void kinectGuiApp::clearMask() {
    kinect.clearMask();
    ofFile maskfile(maskFilename);
    if (maskfile.exists())
        maskfile.remove();
}

//--------------------------------------------------------------
void kinectGuiApp::update(){
    getCurVideo().update();
    kinect.update();
}

//--------------------------------------------------------------
void kinectGuiApp::draw(){
    float w = ofGetWidth();
    float h = ofGetWindowHeight();

    ofBackgroundGradient(bgColor1, bgColor2);

    if (showVideo)
        getCurVideo().draw(0,0,w,h);

    drawKinectImages();

    if (showPointCloud) {
        easyCam.begin();
        kinect.drawPointCloud();
        easyCam.end();
    }

    if (showBlobs) {
        kinect.drawBlobs(0,0,w,h);
    }

    if (showGui) {
        guiApp.draw();
        guiKinect.draw();
        guiImages.draw();
    }
}

void kinectGuiApp::drawKinectImages() {
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

//--------------------------------------------------------------
void kinectGuiApp::keyPressed(int key){
    if (key == 'h') { showGui = !showGui; }
    if (key == 'f') { ofToggleFullscreen(); }
    if (key == 's') { saveSettings(); }
    if (key == 'l') { loadSettings(); }
    if (key == 'g') { grabMask(); }
    if (key == 'p') { playVideo(); }
    if (key == 'P') { pauseVideo(); }
    if (key == 'c') { cueNextVideo(); }
    if (key == 'n') { playNextVideo(); }
}

//--------------------------------------------------------------
void kinectGuiApp::keyReleased(int key){

}

//--------------------------------------------------------------
void kinectGuiApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void kinectGuiApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectGuiApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectGuiApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectGuiApp::windowResized(int w, int h){
    status = ofToString(w) + "x" + ofToString(h);
    guiApp.setPosition(ofGetWidth()-guiApp.getShape().width-10, 10);
    guiImages.setPosition(10,10);
    guiKinect.setPosition(10+guiImages.getShape().width+10,10);
    guiImages.minimizeAll();
    guiKinect.minimizeAll();

}

//--------------------------------------------------------------
void kinectGuiApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void kinectGuiApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void kinectGuiApp::exit() {
//    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();

    loadButton.removeListener(this, &kinectGuiApp::loadSettings);
    saveButton.removeListener(this, &kinectGuiApp::saveSettings);
    grabMaskButton.removeListener(this, &kinectGuiApp::grabMask);

    // clean up midi
	midiIn.closePort();
	midiIn.removeListener(this);

}


