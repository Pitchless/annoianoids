#include "kinectGuiApp.h"
#include "photon.h"

//--------------------------------------------------------------
void kinectGuiApp::setup(){
    ofSetWindowTitle("Annoyanoids");
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetWindowShape(1280, 960);
    bg.setup("bg");

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
    // enum ofPrimitiveMode{
    //OF_PRIMITIVE_TRIANGLES
    //OF_PRIMITIVE_TRIANGLE_STRIP
    //OF_PRIMITIVE_TRIANGLE_FAN
    //OF_PRIMITIVE_LINES
    //OF_PRIMITIVE_LINE_STRIP
    //OF_PRIMITIVE_LINE_LOOP
    //OF_PRIMITIVE_POINTS

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

    joyAxisLeftX  = 0.0;
    joyAxisLeftY  = 0.0;
    joyAxisRightX = 0.0;
    joyAxisRightY = 0.0;
    showJoystick.set("Show Joystick", false);
    joyDeadzone.set("Joystick Deadzone", 0.1, 0.0, 1.0);
	ofxGamepadHandler::get()->enableHotplug();
	//CHECK IF THERE EVEN IS A GAMEPAD CONNECTED
	if(ofxGamepadHandler::get()->getNumPads()>0){
        ofxGamepad* pad = ofxGamepadHandler::get()->getGamepad(0);
        ofAddListener(pad->onAxisChanged, this, &kinectGuiApp::axisChanged);
        ofAddListener(pad->onButtonPressed, this, &kinectGuiApp::buttonPressed);
        ofAddListener(pad->onButtonReleased, this, &kinectGuiApp::buttonReleased);
	}

    kinect.setup();
    setupGui();
    loadSettings();
    kinect.connect();

    cueVideo(0);

    world.setup(VIEW_W, VIEW_H);
    world.gravityY = 2.3;
    bg.world = &world;
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

    if (msg.channel == 10 && msg.velocity > 0 && msg.pitch == 60) {
        cueNextVideo();
    }
    if (msg.channel == 10 && msg.velocity > 0 && msg.pitch == 44) {
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
    vector<string> names;
    for (size_t i=0; i < dir.numFiles(); i++) {
        names.push_back(dir.getPath(i));
    }
    sort(names.begin(), names.end());

    int num_loaded = 0;
    for (size_t i=0; i < names.size(); i++) {
        if ( addVideo(names[i]) ) { num_loaded++; }
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
    vid.setLoopState(OF_LOOP_NONE);
    videos.push_back(vid);
    return true;
}

void kinectGuiApp::playVideo() {
    getCurVideo().play();
    ofLogNotice() << "Playing video: " << getCurVideo().getMoviePath();
}

void kinectGuiApp::togglePlayVideo() {
    ofVideoPlayer vid = getCurVideo();
    if ( vid.isPaused() ) { playVideo(); }
    else { pauseVideo(); }
}

void kinectGuiApp::pauseVideo() {
    getCurVideo().setPaused(true);
    ofLogNotice() << "Pause video: " << getCurVideo().getMoviePath();
}

void kinectGuiApp::cueNextVideo() {
    //getCurVideo().stop();
    //iCurVideo++;
    int num = iCurVideo + 1;
    //if ( iCurVideo > videos.size()-1 ) { iCurVideo = 0; }
    if ( num > videos.size()-1 ) { num = 0; }
    //videos[iCurVideo].play();
    //videos[iCurVideo].setPaused(true);
    //videos[iCurVideo].firstFrame();
    //ofLogNotice() << "Cue video: " << getCurVideo().getMoviePath();
    cueVideo(num);
}

void kinectGuiApp::cueVideo(int num) {
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
    showBlobs = false;
    showMain = false;
    if (num == 0) {
        kinect.lineColor.set(ofColor(0,0,10,32));
    }
    else if (num == 1) {
        kinect.lineColor.set(ofColor(0,201,0,50));
    }
    else if (num == 2) {
        kinect.lineColor.set(ofColor(0,201,0,50));
    }
    else if (num == 4) {
        kinect.lineColor.set(ofColor(10,0,23,23));
    }
    else {
        kinect.lineColor.set(ofColor(255,255,255,23));
    }
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
    //ofxGuiSetFont("verdana.ttf", 10);
    ofxGuiSetFont("Hyperspace Bold.otf", 10);
    
    // Note: The panels will get repositioned in windowResized
    guiApp.setup("KinectGui");
    guiApp.add( fpsSlider.setup("FPS", 60) );
    guiApp.add( reConnectButton.setup("Connect") );
    guiApp.add( loadButton.setup("Load") );
    guiApp.add( saveButton.setup("Save") );
    guiApp.add( grabMaskButton.setup("Grab Mask") );
    //guiApp.add( clearMaskButton.setup("Clear Mask") );
    //guiApp.add( playVideoButton.setup("Play Video") );
    //guiApp.add( pauseVideoButton.setup("Pause Video") );
    //guiApp.add( cueNextVideoButton.setup("Cue Next Video") );
    //guiApp.add( nextVideoButton.setup("Play Next Video") );
    appParams.setName("Display");
    appParams.add( showGui.set("Show Gui", true) );
    //appParams.add( showJoystick );
    //appParams.add( joyDeadzone );
    appParams.add( showPointCloud.set("Show Point Cloud", false) );
    appParams.add( pointMode );
    appParams.add( bPointColor );
    appParams.add( showColorImg.set("RGB", false) );
    appParams.add( showDepthImg.set("Depth", false) );
    appParams.add( showMaskImg.set("Mask", false) );
    appParams.add( showStencilImg.set("Stencil", false) );
    appParams.add( showGrayImg.set("Gray", false) );
    appParams.add( showBlobs.set("Show Blobs", true) );
    appParams.add( bAutoAdd.set("Auto Add", false) );
    appParams.add( autoAddRate.set("Auto Add Rate", 3, 1, 10) );
    //appParams.add( showVideo );
    appParams.add( showWorld );
    appParams.add( mainRotation );
    appParams.add( scale );
    appParams.add( showMain );
    appParams.add( mainAlpha );
    appParams.add( mainHue );
    appParams.add( mainSaturation );
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

void kinectGuiApp::connect() { kinect.reConnect(); }

//--------------------------------------------------------------
void kinectGuiApp::loadSettings() {
    guiApp.loadFromFile("settings.xml");
    guiKinect.loadFromFile("kinect.xml");
    bg.gui.loadFromFile("bg.xml");
    world.gui.loadFromFile("world.xml");
    kinect.loadMask(maskFilename);
}

void kinectGuiApp::saveSettings() {
    guiApp.saveToFile("settings.xml");
    guiKinect.saveToFile("kinect.xml");
    bg.gui.saveToFile("bg.xml");
    world.gui.saveToFile("world.xml");
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
    if (numBlobs > 0) {
          static int last = 0;
          int elapsed = int(ofGetElapsedTimef());
	  if ((elapsed % autoAddRate) == 0 && last != elapsed) {
	    last = elapsed;
	    if (bAutoAdd) addShiz();
	  }
    }

    /* TODO: fix for floats
    if (joyAxisLeftY != 0) {
        ofColor c = kinect.lineColor.get();
        int foo = -4*joyAxisLeftY;
        //ofLogNotice() << "joy: " << joyAxisLeftY << " foo: " << foo;
        //c[3] += int(-2*joyAxisLeftY); // -1 reverse negative is up on stick
        c[3] = ofClamp(c[3] + foo, 0, 255);
        //c.clamp();
        kinect.lineColor.set(c);

        mainAlpha = ofClamp(mainAlpha+foo, 0, 1.0);
    }
    */
    //if (joyAxisRightX != 0) {
    //    mainHue = ofClamp(mainHue+(-4*joyAxisRightX), 0, 255);
    //}
    //if (joyAxisRightY != 0) {
    //    mainSaturation = ofClamp(mainSaturation+(-4*joyAxisRightY), 0, 255);
    //}

    world.update();

    // Copy the kinect grey image into our video layer
    unsigned char* newPix = imgMain.getPixels();
    unsigned char* pix = kinect.grayImg.getPixels();
    int numPix = kinect.grayImg.getWidth() * kinect.grayImg.getHeight();
    ofColor col(0,0,0,0);
    ofColor blank(0,0,0,0);
    for ( int i=0; i<numPix; i++ ) {
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
void kinectGuiApp::draw(){
    float w = VIEW_W;
    float h = VIEW_H;

    bg.draw();

    ofPushStyle();
    ofPushMatrix();
      ofTranslate((w/2.0), (h/2.0));
      ofRotate(mainRotation);
      ofTranslate(-(w/2.0), -(h/2.0));
      ofScale(scale, scale, 0.0);

      //if (showVideo)
      //    getCurVideo().draw(0,0,w,h);

      drawKinectImages();

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

    if (showJoystick)
        ofxGamepadHandler::get()->draw(42,80);

    if (showGui) {
        bg.gui.draw();
        guiApp.draw();
        guiKinect.draw();
        guiImages.draw();
	world.gui.draw();
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

void kinectGuiApp::drawPointCloud() {
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

    //int step = 2;
    //for(int y = 0; y < h; y += step) {
    //    for(int x = 0; x < w; x += step) {
    //        if(kinect.kinect.getDistanceAt(x, y) > 0) {
    //            //mesh.addVertex(kinect.kinect.getWorldCoordinateAt(x, y));
    //            //mesh.addColor(kinect.kinect.getColorAt(x,y));
    //            mesh = ofMesh::box(1.0, 1.0, 1.0);
    //            ofPushMatrix();
    //            ofTranslate(kinect.kinect.getWorldCoordinateAt(x, y));
    //            //mesh.draw();
    //            mesh.drawWireframe();
    //            ofPopMatrix();
    //        }
    //    }
    //}

    glDisable(GL_DEPTH_TEST);
    ofPopMatrix();
}

//--------------------------------------------------------------
void kinectGuiApp::keyPressed(int key){
    if (key == '\t') { showGui = !showGui; }
    if (key == 'F') { ofToggleFullscreen(); }
    if (key == 'S') { saveSettings(); }
    if (key == 'L') { loadSettings(); }
    if (key == 'g') { grabMask(); }
    /*
    if (key == 'p') { playVideo(); }
    if (key == 'P') { pauseVideo(); }
    //if (key == ' ') { togglePlayVideo(); }
    if (key == ' ') { showBlobs = false; showMain = false; }
    if (key == 'C') { cueNextVideo(); }
    if (key == 'N') { playNextVideo(); }
    if (key == '1') { cueVideo(0); }
    if (key == '2') { cueVideo(1); }
    if (key == '3') { cueVideo(2); }
    if (key == '4') { cueVideo(3); }
    if (key == '5') { cueVideo(4); }
    if (key == '6') { cueVideo(5); }
    if (key == '7') { cueVideo(6); }
    if (key == '8') { cueVideo(7); }
    if (key == '9') { cueVideo(8); }
    if (key == '0') { cueVideo(9); }
    */
    else if (key == 'g') { kinect.lineColor.set(ofColor(0,230,0,32)); }
    //else if (key == 'b') { kinect.lineColor.set(ofColor(0,0,200,32)); }
    else if (key == 'y') { kinect.lineColor.set(ofColor(200,200,0,32)); }
    else if (key == 'm') { showBlobs = false; showMain = true; }

    if(key == 'C') { world.clear(); }
    if(key == 'c') { world.addCircle(mouseX/scale, mouseY/scale); }
    if(key == 'b') { world.addRect(mouseX/scale, mouseY/scale); }
    if(key == 'a') { world.addAsteroid(mouseX/scale, mouseY/scale); }
    if(key == 'n') { addShiz(); }
    if(key == '1') {
      world.addSprite(mouseX/scale, mouseY/scale, "600px-Smiley_svg.png");
    }
    if(key == '2') {
      world.addSprite(mouseX/scale, mouseY/scale, "2883994.png");
    }
    if(key == '3') {
      float x = mouseX/scale;
      float y = mouseY/scale;
      world.addSprite(x, y, "qiBAkjkgT.png", 40, 2.2, 2.0, 0.02, 0.6);
    }
    if(key == '4') {
      float x = mouseX/scale;
      float y = mouseY/scale;
      world.addBox(x, y, "424956_10151243754230300_1382486128_n.png", 3, 2.2, 1.0, 0.5, 1.6);
    }
    if(key == '5') {
      float x = mouseX/scale;
      float y = mouseY/scale;
      world.addBox(x, y, "tumblr_lnxy5xxaWS1qm9sn7o1_500.gif", 4, 0.6, 1.0, 0.5, 1.6);
    }
    if(key == 'p') {
      PhotonPtr p = PhotonPtr(new Photon);
      p->setup(world.getB2World(), mouseX/scale, mouseY/scale);
      world.add((StuffPtr)p); 
      p->setVelocity(ofRandom(-10, 10), ofRandom(-10, 10));
    }
}

void kinectGuiApp::addShiz() { addShiz(int(ofRandom(1,6))); };

void kinectGuiApp::addShiz(int shiz) {
    float x = ofRandom(0.0, world.width);
    float y = -200;
    ofLogNotice() << "SHIZ " << shiz << " " << x << "x" << y;
    if(shiz == 1) {
      world.addSprite(x, y, "smiley_yell.png");
      return;
    }
    if(shiz == 2) {
      world.addSprite(x, y, "smiley_pink.png");
      return;
    }
    if(shiz == 3) {
      world.addSprite(x, y, "axe.png", 40, 2.2, 2.0, 0.02, 0.6);
      return;
    }
    if(shiz == 4) {
      world.addBox(x, y, "raggy1.png", 3, 2.2, 1.0, 0.5, 1.6);
      return;
    }
    if(shiz == 5) {
      world.addBox(x, y, "dildo.gif", 4, 0.6, 1.0, 0.5, 1.6);
      return;
    }
    /*
    if(shiz == "p") {
      PhotonPtr p = PhotonPtr(new Photon);
      p->setup(world.getB2World(), mouseX/scale, mouseY/scale);
      world.add((StuffPtr)p); 
      p->setVelocity(ofRandom(-10, 10), ofRandom(-10, 10));
    }
    */
  
};

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
    //world.addCircle(mouseX/scale, mouseY/scale);
    //world.addRect(mouseX/scale, mouseY/scale);
    //world.addAsteroid(mouseX/scale, mouseY/scale);
}

//--------------------------------------------------------------
void kinectGuiApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
// ofxGamepad events

void kinectGuiApp::axisChanged(ofxGamepadAxisEvent& e) {
	//ofLogNotice() << "AXIS " << e.axis << " VALUE " << ofToString(e.value) << endl;
    float val = e.value;
    if ( !(val > joyDeadzone || val < -joyDeadzone) ) {
        val = 0.0;
    }
	if ( e.axis == 0 ) { joyAxisLeftX  = val; }
    if ( e.axis == 1 ) { joyAxisLeftY  = val; }
    if ( e.axis == 3 ) { joyAxisRightX = val; }
    if ( e.axis == 4 ) { joyAxisRightY = val; }
}

void kinectGuiApp::buttonPressed(ofxGamepadButtonEvent& e) {
  /*
	//ofLogNotice() << "BUTTON " << e.button << " PRESSED" << endl;
	if (e.button == 1) { showBlobs = !showBlobs; } // B
	if (e.button == 3) {
        kinect.bFill = !kinect.bFill;
        if (kinect.bFill && kinect.lineColor.get()[3] > 100) {
            ofColor c = kinect.lineColor.get();
            c[3] = 100;
            kinect.lineColor.set(c);
        }
    }
    else if (e.button == 4) { kinect.lineColor.set(ofColor(255,255,255,32)); } // left shoulder
    else if (e.button == 5) { kinect.lineColor.set(ofColor(0,0,0,32)); } // left shoulder
    */
}

void kinectGuiApp::buttonReleased(ofxGamepadButtonEvent& e) {
	//ofLogNotice() << "BUTTON " << e.button << " RELEASED" << endl;
}

//--------------------------------------------------------------
void kinectGuiApp::windowResized(int w, int h){
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


