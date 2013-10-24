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