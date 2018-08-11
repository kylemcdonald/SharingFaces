#define INSTALL
//#define USE_WHITE_POINT

#include "ofMain.h"
#include "SharingFacesUtils.h"

const int installWidth = 1080, installHeight = 1920;
const int camWidth = 1920, camHeight = 1080;

class ofApp : public ofBaseApp {
public:
    ofVideoGrabber cam;
    ofShader shader;
    FaceOverlay overlay;
    ofxFaceTrackerThreaded tracker;
    BinnedData<FaceTrackerData> data;
    FaceCompare faceCompare;
    MultiThreadedImageSaver imageSaver;
    
    bool rotate;
    int binSize;
    float neighborRadius;
    int neighborCount;
    ofImage rotated;
    
    FaceTrackerData nearestData;
    string lastLabel;
    ofImage similar;
    ofVec3f whitePoint;
    
    Hysteresis presence;
    FadeTimer presenceFade, faceFade;
    vector< pair<ofVec2f, FaceTrackerData> > currentData;
    
    void setup() {
        useSharedData();
#ifdef INSTALL
        ofLogToFile("../local/log.txt");
#endif
        loadSettings();
        
        tracker.setup();
        tracker.setHaarMinSize(175);
        tracker.setRescale(.25);
        tracker.setIterations(3);
        tracker.setTolerance(2);
        tracker.setClamp(3);
        tracker.setAttempts(4);
        
        cam.setup(camWidth, camHeight, false);
        
        if(rotate) {
            data.setup(camHeight, camWidth, binSize);
        } else {
            data.setup(camWidth, camHeight, binSize);
        }
        loadMetadata(data);
        presence.setDelay(0, 4);
        presenceFade.setLength(4, .1);
        presenceFade.start();
        faceFade.setLength(0, 30);
        faceFade.start();
        shader.load("shaders/colorbalance.vs", "shaders/colorbalance.fs");
        ofDisableAntiAliasing();
        glPointSize(2);
        ofSetLineWidth(3);
        ofSetLogLevel(OF_LOG_VERBOSE);
    }
    void exit() {
        imageSaver.exit();
        tracker.waitForThread();
    }
    void loadSettings() {
#ifdef INSTALL
        rotate = true;
#else
        rotate = false;
#endif
        binSize = 10;
        neighborRadius = 20;
        neighborCount = 100;
    }
    void checkScreenSize() {
        if(ofGetWindowHeight() != installHeight ||
           ofGetWindowWidth() != installWidth) {
            ofSetFullscreen(false);
            ofSetFullscreen(true);
        }
    }
    void update() {
#ifdef INSTALL
        checkScreenSize();
#endif
        cam.update();
        if(cam.isFrameNew()) {
            ofPixels& pixels = cam.getPixels();
            if(rotate) {
                ofxCv::transpose(pixels, rotated);
            } else {
                ofxCv::copy(pixels, rotated);
            }
            Mat rotatedMat = toCv(rotated);
            if(tracker.update(rotatedMat))  {
                ofVec2f position = tracker.getPosition();
                vector<FaceTrackerData*> neighbors = data.getNeighborsCount(position, neighborCount);
                FaceTrackerData curData;
                curData.load(tracker);
                if(!neighbors.empty()) {
                    nearestData = *faceCompare.nearest(curData, neighbors);
                    if(nearestData.label != lastLabel) {
                        similar.load(nearestData.getImageFilename());
#ifdef USE_WHITE_POINT
                        whitePoint = getWhitePoint(similar);
#endif
                    }
                    lastLabel = nearestData.label;
                }
                if(faceCompare.different(curData, currentData) && faceCompare.different(curData, neighbors)) {
                    saveFace(curData, rotated);
                    currentData.push_back(pair<ofVec2f, FaceTrackerData>(position, curData));
                }
            }
            presence.update(tracker.getFound());
            if(presence.wasTriggered()) {
                presenceFade.stop();
                faceFade.stop();
            }
            if(presence.wasUntriggered()) {
                for(int i = 0; i < currentData.size(); i++) {
                    data.add(currentData[i].first, currentData[i].second);
                }
                currentData.clear();
                presenceFade.start();
                faceFade.start();
            }
        }
    }
    void draw() {
        ofBackground(255);
#ifdef TARGET_OSX
        CGDisplayHideCursor(NULL);
#else
        ofHideCursor();
#endif
        ofSetColor(255);
        if(similar.isAllocated()) {
#ifdef USE_WHITE_POINT
            shader.begin();
            shader.setUniformTexture("tex", similar, 0);
            shader.setUniform3fv("whitePoint", (float*) &whitePoint);
            similar.draw(0, 0);
            shader.end();
#else
            similar.draw(0, 0);
#endif
        }
        ofPushStyle();
        if(presenceFade.getActive()) {
            ofSetColor(0, ofMap(presenceFade.get(), 0, 1, 0, 128));
            ofFill();
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
            ofSetColor(255, ofMap(presenceFade.get(), 0, 1, 0, 32));
            data.drawBins();
            ofSetColor(255, ofMap(presenceFade.get(), 0, 1, 0, 64));
            data.drawData();
        }
        ofSetColor(255, 64);
        ofNoFill();
        if(!tracker.getFound()) {
            ofDrawCircle(tracker.getPosition(), 10);
        }
        ofSetColor(255, 96 * faceFade.get());
        overlay.draw(tracker);
        ofPopStyle();
        
#ifndef INSTALL
        drawFramerate();
#endif
    }
    void keyPressed(int key) {
        if(key == 'f') {
            ofToggleFullscreen();
        }
    }
    
    void saveFace(FaceTrackerData& data, ofImage& img) {
        string basePath = ofGetTimestampString("%Y.%m.%d/%H.%M.%S.%i");
        data.save("metadata/" + basePath + ".face");
        imageSaver.saveImage(img.getPixels(), data.getImageFilename());
    }
};

int main() {
    ofSetupOpenGL(1080, 1080, OF_FULLSCREEN);
    ofRunApp(new ofApp());
}
