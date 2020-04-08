#pragma once

#include "ofMain.h"
#include "ofxPeertalk.h"
#include "ARKitFaceTracker.h"
#include "FaceTrackerData.h"
#include "BinnedData.h"
#include "FaceCompare.h"
#include "ImageSaver.h"
#include "ofxBlackMagicGrabber.h"
#include "ofxTiming.h"
#include "BlurDetection.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
		
    // Peertalk
    ofxPeertalk peertalk;
    string peertalkStatus;
    void onPeertalkMessageReceived(string & message);
    void onPeertalkStatusDidChange(string & newStatus);
    void onPeertalkFacePRSDataReceived(map<string, float> & faceData);
    void onPeertalkFaceExpressionDataReceived(map<string, float> & faceData);
    
    // Face Data
    ARKitFaceTracker arkitFaceTracker;
    bool newFaceDataAvailable;
    vector< pair<ofVec2f, FaceTrackerData> > currentData;
    vector<float> tmpExpressionData;
    
    // Face Compare
    FaceCompare faceCompare;
    BinnedData<FaceTrackerData> data;
    int binSize;
    float neighborRadius;
    int neighborCount;
    FaceTrackerData nearestData;
    string lastLabel;
    ofImage similar;
    
    // Timing & Session Management
    Hysteresis presence;
    FadeTimer presenceFade, faceFade;
    bool faceCurrentlyDetected;
    
    // Cameras & Video
    ofVideoGrabber videoGrabber;
    ofxBlackMagicGrabber blackmagicGrabber;
    ofBaseVideoGrabber* cam;
    float videoDelay;
    
    // Image Saving
    void saveFace(FaceTrackerData& data, ofImage& img);
    ofImage cameraImage;
    MultiThreadedImageSaver imageSaver;
    
    // Opt-In
    bool faceSavingPermissionGranted;
    float lastOptInTime;
    ofImage cameraImageFrozen; // NEW 9/19
    vector< pair<ofVec2f, FaceTrackerData> > mostRecentData; // NEW 9/19
    
    // Debug States
    bool cameraLiveView;
    
    // Attract State
    void drawAttractState();
    void generateAttractStateImages();
    void assignAttractStateImages(bool mostRecent);
    DelayTimer attractLoopTimerLeft;
    DelayTimer attractLoopTimerRight;
    ofVec2f positionAttract;
    ofVec3f orientationAttract;
    float scaleAttract;
    vector<float> expressionsAttract;
    ARKitFaceTracker faceTrackerAttract;
    ofImage similarAttractLeft, similarAttractRight;
    vector<FaceTrackerData*> nearestDataArrayAttract;
    int carouselCounter;
    int attractCounter; // NEW - 9/19
    void generateAttractStateImagesMostRecent(); // NEW 9/19
    
    // Random & Hash
    void generateRandomSeeds();
    float pxR, pyR, oxR, oyR, ozR, sR;
    vector<float> expR;
    
    // DeBlur
    ofVec2f currentPosition;
    ofVec2f lastPosition;
    float frameDist;
    float frameDistThreshold;
    
    // Utils
    void useSharedData();
    void loadMetadata(vector<FaceTrackerData>& dataM);
    void loadMetadata(BinnedData<FaceTrackerData>& binned);
    uint64_t startTime;
    void startTimer();
    uint64_t checkTimer();
    RateTimer appTimer;
    
    // GUI
    void drawGUI();
    void drawSavedFaceData();
    void drawCameraImageOverlay();
    void drawCurrentFaceInfo(int xPosition, int yPosition);
    ofRectangle guiBox;
    ofTrueTypeFont guiFont;
    ofVec2f nearestPosition;
    int numSaved;
    
    // Config
    ofJson config;
    
    // Safe Restart
    void sharingFacesRestart(string device);
    void checkRestartTimer();
    int restartState;
    float restartGap;
    float restartTimeBegin;
    string currentTimeString;
    string timeToRestartString;
};
