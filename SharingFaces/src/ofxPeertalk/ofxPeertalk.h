#pragma once

#ifndef ofxPeertalk_h
#define ofxPeertalk_h

#include "ofMain.h"

class PTManager;

class ofxPeertalk{
public:
    ofxPeertalk() {}
    ~ofxPeertalk();
    ofxPeertalk(const ofxPeertalk &mom);
    ofxPeertalk& operator=(const ofxPeertalk &mom);
    /// for operator= and copy constructor
    ofxPeertalk& copy(const ofxPeertalk &other);
    
    PTManager *_ptManager;
    
    void startLookingForDevices();
    static void processMessage(const char*);
    static void processFacePRSData(map<string, float>);
    static void processFaceExpressionData(map<string, float>);
    static void processPeertalkStatusChange(const char*);
    static ofEvent<string> peertalkMessageReceived;
    static ofEvent<map<string, float>> peertalkFacePRSDataReceived;
    static ofEvent<map<string, float>> peertalkFaceExpressionDataReceived;
    static ofEvent<string> peertalkStatusDidChange;
    
};

#endif
