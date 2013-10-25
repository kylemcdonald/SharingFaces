#pragma once

#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "FaceTrackerData.h"
#include "BinnedData.h"

using namespace ofxCv;
using namespace cv;

void useSharedData() {
	ofSetDataPathRoot("../../../../../SharedData/");
}

void drawFramerate() {
	ofDrawBitmapStringHighlight(ofToString((int) ofGetFrameRate()), 10, 20);
}

float getMaximumDistance(ofVec2f& position, vector<ofVec2f*> positions) {
	float maximumDistance = 0;
	for(int i = 0; i < positions.size(); i++) {
		float distance = position.distanceSquared(*positions[i]);
		if(distance > maximumDistance) {
			maximumDistance = distance;
		}
	}
	return sqrt(maximumDistance);
}

float getMinimumDistance(ofVec2f& position, vector<ofVec2f*> positions) {
	float minimumDistance = 0;
	for(int i = 0; i < positions.size(); i++) {
		float distance = position.distanceSquared(*positions[i]);
		if(i == 0 || distance < minimumDistance) {
			minimumDistance = distance;
		}
	}
	return sqrt(minimumDistance);
}