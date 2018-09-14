#include "ofMain.h"
#include "ofxBlackMagicGrabber.h"
#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
    ofVideoGrabber videoGrabber;
    ofxBlackMagicGrabber blackmagicGrabber;
    ofBaseVideoGrabber* cam;
    
    FaceOverlay overlay;
    ofxFaceTrackerThreaded tracker;
    BinnedData<FaceTrackerData> data;
    FaceCompare faceCompare;
    MultiThreadedImageSaver imageSaver;
    
    int rotate;
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
    
    ofJson config;
    
    void setup() {
        useSharedData();
        config = ofLoadJson("config.json");
        if(config["log"]) {
            ofLogToFile("../local/log.txt");
        }
        
        binSize = 10;
        neighborRadius = 20;
        neighborCount = 100;
        
        tracker.setup();
        tracker.setHaarMinSize(175);
        tracker.setRescale(.25);
        tracker.setIterations(3);
        tracker.setTolerance(2);
        tracker.setClamp(3);
        tracker.setAttempts(4);
        
        rotate = config["camera"]["rotate"];
        int camWidth = config["camera"]["width"];
        int camHeight = config["camera"]["height"];
        
        if(config["camera"]["device"] == "blackmagic") {
            blackmagicGrabber.setDesiredFrameRate(config["camera"]["framerate"]);
            blackmagicGrabber.setup(camWidth, camHeight);
            cam = &blackmagicGrabber;
        } else {
            videoGrabber.setup(camWidth, camHeight, false);
            cam = &videoGrabber;
        }
        
        if(rotate == 0 || rotate == 180) {
            data.setup(camWidth, camHeight, binSize);
        } else {
            data.setup(camHeight, camWidth, binSize);
        }
        loadMetadata(data);
        presence.setDelay(0, 4);
        presenceFade.setLength(4, .1);
        presenceFade.start();
        faceFade.setLength(0, 30);
        faceFade.start();
        ofDisableAntiAliasing(); // why is this here?
        glPointSize(2);
        ofSetLineWidth(3);
        ofSetLogLevel(OF_LOG_VERBOSE);
        
        checkScreenSize();
    }
    void exit() {
        cam->close();
        imageSaver.exit();
        tracker.waitForThread();
    }
    void checkScreenSize() {
        int curx = ofGetWindowPositionX();
        int cury = ofGetWindowPositionY();
        int screenx = config["screen"]["x"];
        int screeny = config["screen"]["y"];
        if(curx != screenx || cury != screeny) {
            ofSetWindowPosition(screenx, screeny);
        }
        bool fullscreen = config["screen"]["fullscreen"];
        ofSetFullscreen(fullscreen);
        if(!fullscreen) {
            int curw = ofGetWindowWidth();
            int curh = ofGetWindowHeight();
            int screenw = config["screen"]["width"];
            int screenh = config["screen"]["height"];
            if(curw != screenw && curh != screenh) {
                ofSetWindowShape(screenw, screenh);
            }
        }
    }
    void update() {
        cam->update();
        if(cam->isFrameNew()) {
            ofPixels& pixels = cam->getPixels();
            ofxCv::rotate90(pixels, rotated, rotate);
            if(config["camera"]["mirror"]) {
                ofxCv::flip(rotated, rotated, 1);
            }
            imageSaver.saveImage(rotated.getPixels(), "../reference/" + ofToString(ofGetFrameNum(), 6) + ".jpg");
            
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
                    }
                    lastLabel = nearestData.label;
                }
                if(faceCompare.different(curData, currentData) && faceCompare.different(curData, neighbors)) {
                    saveFace(curData, rotated);
                    currentData.push_back(pair<ofVec2f, FaceTrackerData>(position, curData));
                }
            }
//            cout << rotatedMat.cols << "x" << rotatedMat.rows << "x" << rotatedMat.channels() << ": " << tracker.getFound() << endl;
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
        ofHideCursor();
        ofSetColor(255);
        if(similar.isAllocated()) {
            similar.draw(0, 0);
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
        
        if(config["debug"]) {
            drawFramerate();
        }
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
    ofSetupOpenGL(1080, 1080, OF_WINDOW);
    ofRunApp(new ofApp());
}
