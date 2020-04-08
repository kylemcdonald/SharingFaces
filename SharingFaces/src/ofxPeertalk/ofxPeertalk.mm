//
//  ofxPeertalk.cpp
//  PeertalkTest
//
//  Created by Elliott Spelman on 2/24/19.
//

#import "ofxPeertalk.h"
#import "PeertalkBridgingHeader.h"
#include <string>

ofEvent<string> ofxPeertalk::peertalkMessageReceived = ofEvent<string>();
ofEvent<map<string, float>> ofxPeertalk::peertalkFacePRSDataReceived = ofEvent<map<string, float>>();
ofEvent<map<string, float>> ofxPeertalk::peertalkFaceExpressionDataReceived = ofEvent<map<string, float>>();
ofEvent<string> ofxPeertalk::peertalkStatusDidChange = ofEvent<string>();


//--------------------------------------------------------------
ofxPeertalk::~ofxPeertalk() {
}

//--------------------------------------------------------------
ofxPeertalk::ofxPeertalk(const ofxPeertalk & mom){
    copy(mom);
}

//--------------------------------------------------------------
ofxPeertalk& ofxPeertalk::operator=(const ofxPeertalk & mom){
    return copy(mom);
}

//--------------------------------------------------------------
ofxPeertalk& ofxPeertalk::copy(const ofxPeertalk& other){
    return *this;
}

//--------------------------------------------------------------
void ofxPeertalk::startLookingForDevices(){
    _ptManager = new PTManager();
    _ptManager->init();
}

//--------------------------------------------------------------
void ofxPeertalk::processMessage(const char* message){
    std::string messageString(message);
    ofNotifyEvent(peertalkMessageReceived, messageString);
}

void ofxPeertalk::processFacePRSData(map<string, float> floatMap) {
    ofNotifyEvent(peertalkFacePRSDataReceived, floatMap);
}

void ofxPeertalk::processFaceExpressionData(map<string, float> floatMap) {
    ofNotifyEvent(peertalkFaceExpressionDataReceived, floatMap);
}

void ofxPeertalk::processPeertalkStatusChange(const char* newStatus) {
    std::string statusString(newStatus);
    cout << newStatus << endl;
    std::string testString = "TEST STRING";
    ofNotifyEvent(peertalkStatusDidChange, testString);
}
