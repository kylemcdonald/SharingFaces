#define USE_BLACKMAGIC

#include "ofApp.h"
#include <iostream>
#include <string>
#include <sstream>


void ofApp::setup(){
    // OF Settings
    ofHideCursor();
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
    ofEnableAlphaBlending();
    useSharedData();
    
    // Config Settings
    config = ofLoadJson("config.json");
    if (config["log"]) {
        ofLogToFile("../local/log.txt");
    }
    int camWidth = config["camera"]["width"];
    int camHeight = config["camera"]["height"];
    
    // Peertalk
    peertalk.startLookingForDevices();
    ofAddListener(ofxPeertalk::peertalkMessageReceived , this, &ofApp::onPeertalkMessageReceived);
    ofAddListener(ofxPeertalk::peertalkFacePRSDataReceived , this, &ofApp::onPeertalkFacePRSDataReceived);
    ofAddListener(ofxPeertalk::peertalkFaceExpressionDataReceived , this, &ofApp::onPeertalkFaceExpressionDataReceived);
    ofAddListener(ofxPeertalk::peertalkStatusDidChange , this, &ofApp::onPeertalkStatusDidChange);
    peertalkStatus = "Peertalk: Not Connected";
    
    // Cameras & Video
    if (config["camera"]["device"] == "blackmagic") {
        blackmagicGrabber.setDesiredFrameRate(config["camera"]["framerate"]);
        blackmagicGrabber.setup(camWidth, camHeight);
        cam = &blackmagicGrabber;
    } else {
        videoGrabber.initGrabber(camWidth, camHeight);
        cam = &videoGrabber;
    }
    cameraImage.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
    similar.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
    cameraImageFrozen.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
    
    // Face Data
    arkitFaceTracker.setup();
    ofVec3f p = ofVec3f(0, 0, 0);
    
    // Face Compare
    newFaceDataAvailable = false;
    binSize = 10;
    neighborRadius = 20;
    neighborCount = 100;
    data.setup(375, 812, binSize); // binning based on iPhone screen dimensions
    loadMetadata(data);
    
    // Timing & Session Management
    faceCurrentlyDetected = false;
    presence.setDelay(0, 0);
    presenceFade.setLength(4, .1);
    presenceFade.start();
    faceFade.setLength(0, 30);
    faceFade.start();
    videoDelay = 0.12;
    
    // Opt-In (toggle with the 'a' key for now)
    faceSavingPermissionGranted = false;
    lastOptInTime = 0;
    
    // Attract State
    attractLoopTimerLeft.setPeriod(9);
    attractLoopTimerRight.setPeriod(1);
    faceTrackerAttract.setup();
    similarAttractLeft.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
    similarAttractRight.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
    carouselCounter = 0;
    attractCounter = 0;
    
    // Random Hash
    generateRandomSeeds();
    
    // DeBlur (Movement Detection)
    frameDist = 0;
    frameDistThreshold = 20;
    
    // GUI
    guiBox.set(0, 0, 200, ofGetHeight());
    guiFont.load(OF_TTF_MONO, 8);
    numSaved = 0;
    
    // Debug States
    cameraLiveView = false;  //(toggle with the 'c' key)
    
    // Safe Restart
    restartState = 0;
    restartGap = 30.0;
    restartTimeBegin = 0;
    timeToRestartString = "07-00-00";
}

void ofApp::exit() {
    cam->close();
    imageSaver.exit();
    // TODO gracefully close peertalk
}

void ofApp::update(){
    cam->update();
    if (cam->isFrameNew()) {
        ofPixels& pixels = cam->getPixels();
        cameraImage.setFromPixels(pixels);
        
        if (newFaceDataAvailable) {
            newFaceDataAvailable = false;
            faceCurrentlyDetected = true;
            ofVec2f position = arkitFaceTracker.getPosition();
            
            // check to see how much they've moved between frames - if it's too much, don't save the face
            currentPosition.set(position);
            frameDist = currentPosition.distance(lastPosition);
            lastPosition.set(position);
            
            vector<FaceTrackerData*> neighbors = data.getNeighborsCount(position, neighborCount);
            FaceTrackerData curData;
            curData.load(arkitFaceTracker);
            if(!neighbors.empty()) {
                nearestData = *faceCompare.nearest(curData, neighbors);
                nearestPosition.set(nearestData.position);
                if (nearestData.label != lastLabel) {
                    similar.load(nearestData.getImageFilename());
                }
                lastLabel = nearestData.label;
            }
            
            // check for visitor opt-in
            if (faceSavingPermissionGranted && (frameDist < frameDistThreshold)) {
                // check to see if the current face is 'different enough' from existing saved faces
                if (faceCompare.different(curData, currentData) && faceCompare.different(curData, neighbors)) {
                    saveFace(curData, cameraImage);
                    currentData.push_back(pair<ofVec2f, FaceTrackerData>(position, curData));
                    numSaved += 1;
                }
            }
        }
        
        presence.update(faceCurrentlyDetected);
        if (presence.wasTriggered()) { // visitor has arrived
            presenceFade.stop();
            faceFade.stop();
        }
        if (presence.wasUntriggered()) { // visitor has left
            for(int i = 0; i < currentData.size(); i++) {
                data.add(currentData[i].first, currentData[i].second);
            }
            mostRecentData = currentData;
            currentData.clear();
            presenceFade.start();
            faceFade.start();
            faceSavingPermissionGranted = false;
            attractCounter = 0; // NEW 9/19
        }
    }
    appTimer.tick();
    checkRestartTimer();
}

void ofApp::draw(){
    if (faceCurrentlyDetected) {
        if (!cameraLiveView){
            similar.draw(0,0); // SCREEN 1
        } else {
            cameraImage.draw(0,0); // SCREEN 1
        }
        cameraImage.draw(1920,0); // SCREEN 2
        similar.draw(1920*2,0); // SCREEN 3
        
        // drawCameraImageOverlay(); // helpful similar/camera comparison for debugging
    }
    
    if (presenceFade.getActive()) {
        drawAttractState();
    }
    
    float optInTimerDist = ofGetElapsedTimef() - lastOptInTime;
    if (optInTimerDist < 1 && optInTimerDist > 0) {
        drawCameraImageOverlay();
         ofPushStyle();
        // ofSetColor(255, 255, 255, ofMap(optInTimerDist, 0, 5, 255, 0));
         cameraImageFrozen.draw(0,0);
        // ofDrawRectangle(0, 0, 1920, 1080);
         ofPopStyle();
    }
    
    if (faceSavingPermissionGranted == true) {
        ofPushMatrix();
        ofTranslate(700, 400);
        ofRotateZDeg(-90);
        ofScale(-1,1,1);
        ofDrawBitmapString("image is being saved.", 50, 1200);
        ofPopMatrix();
    }
    
    // drawSavedFaceData(); // helpful saved data visualization for debugging
    drawGUI();
}

void ofApp::drawAttractState() {
    cameraImage.draw(0,0);
    if (attractLoopTimerLeft.tick()) {
        if (attractCounter < 2 && mostRecentData.size() > 1) {
            generateAttractStateImagesMostRecent();
            assignAttractStateImages(true);
        } else {
            generateAttractStateImages();
            assignAttractStateImages(false);
        }
//        generateAttractStateImages();
//        assignAttractStateImages(false);
    }
    if (attractLoopTimerRight.tick()) {
        if (attractCounter < 2 && mostRecentData.size() > 1) {
            assignAttractStateImages(true);
        } else {
            assignAttractStateImages(false);
        }
//        assignAttractStateImages(false);
    }
    similarAttractLeft.draw(1920,0);
    similarAttractRight.draw(1920*2,0);
}

void ofApp::generateAttractStateImages() {
    float t = ofGetElapsedTimef();
    float px = ofMap(sin(t/pxR), -1, 1, 0, 250);
    float py = ofMap(cos(t/pyR), -1, 1, 300, 500);
    float rx = ofMap(sin(t/oxR), -1, 1, -12, 12);
    float ry = ofMap(sin(t/oyR), -1, 1, 15, -18);
    float rz = ofMap(sin(t/ozR), -1, 1, -15, 15);
    positionAttract = ofVec2f(px,py);
    orientationAttract = ofVec3f(rx, ry, rz);
    scaleAttract = ofMap(sin(t/pxR), -1, 1, -0.5, -0.25);
    expressionsAttract.clear();
    for (int i = 0; i < 52; i++) {
        float ei = sin(t/expR[i]) * 0.5 + 0.5;
        expressionsAttract.push_back(ei);
    }
    
    // populate faceTracker with artificial data
    faceTrackerAttract.setPosition(positionAttract);
    faceTrackerAttract.setOrientation(orientationAttract);
    faceTrackerAttract.setScale(scaleAttract);
    faceTrackerAttract.setExpressions(expressionsAttract);
    
    vector<FaceTrackerData*> neighborsAttract = data.getNeighborsCount(positionAttract, 50);
    FaceTrackerData curDataAttract;
    curDataAttract.load(faceTrackerAttract);
    if(!neighborsAttract.empty()) {
        nearestDataArrayAttract = faceCompare.nearestX(10, curDataAttract, neighborsAttract);
        for (int j = 0; j < nearestDataArrayAttract.size(); j++) {
            string label = nearestDataArrayAttract[j]->label;
        }
    } else {
        cout << "no neighbors, sorry" << endl;
    }
    
    attractCounter += 1;
}

void ofApp::assignAttractStateImages(bool mostRecent) {
    if (nearestDataArrayAttract.size() > 0) {
        if (mostRecent == false) {
            similarAttractLeft.load(nearestDataArrayAttract[0]->getImageFilename());
        }
        carouselCounter += 1;
        if (carouselCounter >= nearestDataArrayAttract.size()) {
            carouselCounter = 0;
        }
        similarAttractRight.load(nearestDataArrayAttract[carouselCounter]->getImageFilename());
    }
}

void ofApp::generateAttractStateImagesMostRecent() {
    FaceTrackerData recentData = mostRecentData[attractCounter].second;
    positionAttract = recentData.position;
    orientationAttract = recentData.orientation;
    scaleAttract = recentData.scale;
    expressionsAttract = recentData.expressions;
    // get filename
    string fname = recentData.getImageFilename();
    // assign left image
    similarAttractLeft.load(fname);
    // get neighbors
    vector<FaceTrackerData*> neighborsAttract = data.getNeighborsCount(positionAttract, 50);
    // get nearestX and load into nearestDataArrayAttract
    if(!neighborsAttract.empty()) {
        nearestDataArrayAttract = faceCompare.nearestX(10, recentData, neighborsAttract);
        //        for (int j = 0; j < nearestDataArrayAttract.size(); j++) {
        //            string label = nearestDataArrayAttract[j]->label;
        //        }
    } else {
        cout << "no neighbors, sorry" << endl;
    }
    attractCounter += 1;
}

void ofApp::drawSavedFaceData() {
    // draw bins and data
    ofPushMatrix();
    ofTranslate(800,500);
    ofRotateZDeg(-90);
    ofScale(-1,1,1);
    data.drawBins();
    data.drawData();
    data.drawCurrentPosition(currentPosition);
    data.drawNearestPosition(nearestPosition);
    ofPopMatrix();
}

void ofApp::drawCameraImageOverlay() {
    ofPushStyle();
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255, 123);
    cameraImage.draw(0,0);
    ofDisableAlphaBlending();
    ofPopStyle();
}

// PEERTALK

void ofApp::onPeertalkMessageReceived(string & message){
    if (message == "faceHasArrived") {
        cout << "faceHasArrived message received" << endl;
        faceCurrentlyDetected = true;
    } else if (message == "faceHasLeft") {
        float time = ofGetElapsedTimef();
        cout << "faceHasLeft message received: " << time << endl;
        faceCurrentlyDetected = false;
    }
}

void ofApp::onPeertalkFacePRSDataReceived(map<string, float> & faceData){
    // pull out screen position from ARKit PRS bundle
    auto pxs = faceData.find("pXs");
    auto pys = faceData.find("pYs");
    auto pzs = faceData.find("pZ");
    
    // reformat position and scale into useful things
    ofVec2f position = ofVec2f(pxs->second, pys->second);
    float scale = pzs->second;
    
    // pull out world orientation from ARKit PRS bundle
    auto rx = faceData.find("rX");
    auto ry = faceData.find("rY");
    auto rz = faceData.find("rZ");
    auto rw = faceData.find("rW");
    
    // reformat orientation into a useful thing
    ofQuaternion o = ofQuaternion(rx->second, ry->second, rz->second, rw->second);
    ofVec3f orientation = o.getEuler();

    // set the local position, orientation, and scale variables to the useful things
    arkitFaceTracker.setPosition(position);
    arkitFaceTracker.setOrientation(orientation);
    arkitFaceTracker.setScale(scale);
}

void ofApp::onPeertalkFaceExpressionDataReceived(map<string, float> & faceData){
    tmpExpressionData.clear();
    for (auto const& expression : faceData) {
        tmpExpressionData.push_back(expression.second);
    }
    
    arkitFaceTracker.setExpressions(tmpExpressionData);
    newFaceDataAvailable = true;
}

void ofApp::onPeertalkStatusDidChange(string & newStatus){
    peertalkStatus = "Peertalk: " + newStatus;
}

// UTILS

void ofApp::saveFace(FaceTrackerData& data, ofImage& img) {
    string basePath = ofGetTimestampString("%Y.%m.%d/%H.%M.%S.%i");
    data.save("metadata/" + basePath + ".face");
    imageSaver.saveImage(img.getPixels(), data.getImageFilename());
}

void ofApp::useSharedData() {
    string dataPathRoot = "../SharedData/shared";
#ifdef TARGET_OSX
    dataPathRoot = "../../../" + dataPathRoot;
#endif
    ofSetDataPathRoot(dataPathRoot);
}

void ofApp::loadMetadata(vector<FaceTrackerData>& dataM) {
    startTimer();
    ofLog() << "loading metadata...";
    ofDirectory allDates("metadata/");
    allDates.listDir();
    int total = 0;
    for(int i = 0; i < allDates.size(); i++) {
        ofDirectory curDate(allDates[i].path());
        curDate.listDir();
        string curDateName = "metadata/" + allDates[i].getFileName() + "/";
        for(int j = 0; j < curDate.size(); j++) {
            FaceTrackerData curData;
            string metadataFilename = curDateName + curDate[j].getFileName();
            curData.load(metadataFilename);
            if(ofFile::doesFileExist(curData.getImageFilename())) {
                dataM.push_back(curData);
                total++;
            } else {
                ofLogWarning() << "couldn't find " << curData.getImageFilename();
                //                ofLogWarning() << "removing metadata " << metadataFilename;
                //                ofFile::removeFile(metadataFilename);
            }
        }
    }
     ofLog() << "done loading " << allDates.size() << " days, " << total << " faces in " << checkTimer() << "ms.";
}

void ofApp::loadMetadata(BinnedData<FaceTrackerData>& binned) {
    ofLog() << "loading metadata in BinnedData function";
    vector<FaceTrackerData> dataM;
    loadMetadata(dataM);
    startTimer();
    for(FaceTrackerData& item : dataM) {
        binned.add(item.position, item);
    }
    ofLog() << "done binning " << dataM.size() << " faces in " << checkTimer() << "ms.";
}

void ofApp::startTimer() {
    startTime = ofGetElapsedTimeMillis();
}

uint64_t ofApp::checkTimer() {
    return ofGetElapsedTimeMillis() - startTime;
}

void ofApp::generateRandomSeeds() {
    pxR = ofRandom(1.0, 2.0);
    pyR = ofRandom(1.0, 2.0);
    oxR = ofRandom(1.0, 2.0);
    oyR = ofRandom(1.0, 2.0);
    ozR = ofRandom(1.0, 2.0);
    sR = ofRandom(1.0, 2.0);
    for (int i = 0; i < 52; i++) {
        expR.push_back(ofRandom(1.0, 2.0));
    }
}

void ofApp::checkRestartTimer() {
    currentTimeString = ofGetTimestampString("%H-%M-%S");
    if (currentTimeString == "07-00-00") {
        if (restartState == 0) {
            restartState = 1;
            sharingFacesRestart("iPhone");
        }
    }
    
    if (restartState == 1) {
        float timeDiff = ofGetElapsedTimef() - restartTimeBegin;
        if (timeDiff > restartGap) {
            restartState = 2;
            sharingFacesRestart("MacMini");
        }
    }
}

void ofApp::sharingFacesRestart(string device) {
    cout << "restarting device: " << device << " at time: " << ofGetElapsedTimef() << endl;
    if (device == "iPhone") {
        restartTimeBegin = ofGetElapsedTimef();
        string command = "echo $PATH; export PATH=$PATH:/usr/local/bin; echo $PATH; ~/Documents/restartiPhone.sh";
        system(command.c_str());
    }
    if (device == "MacMini") {
        ofExit();
    }
}

// KEY PRESSED RESPONDERS (Used for granting opt-in permission or not)

void ofApp::keyPressed(int key){
    cout << key << endl;
    switch (key){
        case 97: // 'a' key
        {
            if (faceSavingPermissionGranted == false && faceCurrentlyDetected == true) {
                ofPixels& pixels = cam->getPixels();
                cameraImageFrozen.setFromPixels(pixels);
                faceSavingPermissionGranted = true;
                lastOptInTime = ofGetElapsedTimef();
            } else if (faceSavingPermissionGranted == true && faceCurrentlyDetected == true) {
                ofPixels& pixels = cam->getPixels();
                cameraImageFrozen.setFromPixels(pixels);
                lastOptInTime = ofGetElapsedTimef();
            }
            break;
            
        }
        case 99: // 'c' key
        {
            cameraLiveView = !cameraLiveView;
            string cameraLiveViewState = cameraLiveView ? "true" : "false";
            cout << "faceSavingPermissionGranted: " << cameraLiveViewState << endl;
            break;
        }
        case 114: // 'r' key
        {
            string command = "echo $PATH; export PATH=$PATH:/usr/local/bin; echo $PATH; ~/Documents/restartiPhone.sh";
            system(command.c_str());
        }
    }
    
    
}


// GUI
void ofApp::drawGUI() {
    drawCurrentFaceInfo(10, 30);
}

void ofApp::drawCurrentFaceInfo(int xPosition, int yPosition) {
    ofVec2f p = arkitFaceTracker.getPosition();
    ofVec3f o = arkitFaceTracker.getOrientation();
    float s = arkitFaceTracker.getScale();
    
    stringstream msg;
    msg << fixed << setprecision(3) << "Position: (" << p.x << ", " << p.y << ")" << endl;
    msg << "Orientation: (" << o.x << ", " << o.y << ", " << o.z << ")" << endl;
    msg << "Scale: (" << s << ")" << endl;
    msg << "Inter-Frame Movement: " << frameDist << endl;
    msg << "NumSaved: " << numSaved << endl;
    msg << "framerate: " << int(appTimer.getFrameRate()) << " fps" << endl;
    msg << "faceCurrentlyDetected State: " << faceCurrentlyDetected << endl;
    msg << "faceSavingPermissionGranted State: " << faceCurrentlyDetected << endl;
    msg << "CameraLiveView State: " << cameraLiveView << endl;
    ofDrawBitmapString(msg.str(), xPosition, yPosition);
    
//    ofPushMatrix();
//    ofTranslate(700, 400);
//    ofRotateZDeg(-90);
//    ofScale(-1,1,1);
//    ofDrawBitmapString(msg.str(), 500, 500);
//    ofPopMatrix();
    
}
