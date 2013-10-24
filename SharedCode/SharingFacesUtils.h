#pragma once

#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "FaceTrackerData.h"

using namespace ofxCv;
using namespace cv;

void useSharedData() {
	ofSetDataPathRoot("../../../../../SharedData/");
}