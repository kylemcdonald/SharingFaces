#pragma once

#include "ofxCv.h"
#include "ofxBlackMagicGrabber.h"
#include "ofxFaceTrackerTHreaded.h"
#include "ofxTiming.h"
#include "FaceTrackerData.h"
#include "BinnedData.h"
#include "FaceCompare.h"
#include "ImageSaver.h"

using namespace ofxCv;
using namespace cv;

#define CLEAN_METADATA

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

void loadMetadata(BinnedData<FaceTrackerData>& data) {
	ofDirectory allDates("metadata/");
	allDates.listDir();
	for(int i = 0; i < allDates.size(); i++) {
		ofDirectory curDate(allDates[i].path());
		curDate.listDir();
		string curDateName = "metadata/" + allDates[i].getFileName() + "/";
		for(int j = 0; j < curDate.size(); j++) {
			FaceTrackerData curData;
			string metadataFilename = curDateName + curDate[j].getFileName();
			curData.load(metadataFilename);
			if(ofFile::doesFileExist(curData.getImageFilename())) {
				data.add(curData.position, curData);
			} else {
				ofLogWarning() << "removing metadata " << metadataFilename;
				ofFile::removeFile(metadataFilename);
			}
		}
	}
}