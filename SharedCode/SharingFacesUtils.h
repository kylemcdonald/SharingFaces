#pragma once

#include "ofxCv.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxTiming.h"
#include "FaceTrackerData.h"
#include "BinnedData.h"
#include "FaceCompare.h"
#include "ImageSaver.h"
#include "FaceOverlay.h"

using namespace ofxCv;
using namespace cv;

void useSharedData() {
    string dataPathRoot = "../../SharedData/shared";
#ifdef TARGET_OSX
    dataPathRoot = "../../../" + dataPathRoot;
#endif
    ofSetDataPathRoot(dataPathRoot);
}

void drawFramerate() {
	ofDrawBitmapStringHighlight(ofToString((int) ofGetFrameRate()), 10, 20);
}

float saturationMetric(const ofColor& color) {
	float y = (color.r + color.g + color.b) / (float) (3);
	float dr = color.r - y, dg = color.g - y, db = color.b - y;
	return dr * dr + dg * dg + db * db;
}

bool bySaturation(const ofColor& a, const ofColor& b) {
	return saturationMetric(a) < saturationMetric(b);
}

int brightnessMetric(const ofColor& color) {
	return (int) color.r + (int) color.g + (int) color.b;
}

bool byBrightness(const ofColor& a, const ofColor& b) {
	return brightnessMetric(a) < brightnessMetric(b);
}

ofVec3f getWhitePoint(ofImage& img) {
	vector<ofColor> all;
	int n = img.getWidth();
	for(int i = 0; i < n; i++) {
		ofColor cur = img.getColor(i, 0);
		all.push_back(cur);
	}
	ofSort(all, byBrightness); // sort by brightness
	all.erase(all.begin(), all.begin() + (n / 4)); // remove first 1/4
	all.erase(all.end() - (n / 4), all.end()); // remove last 1/4
	ofSort(all, bySaturation); // sort by saturation
	all.resize(all.size() / 2); // 50% least saturated
	ofVec3f whitePoint;
	n = all.size();
	for(int i = 0; i < n; i++) {
		whitePoint[0] += all[i].r;
		whitePoint[1] += all[i].g;
		whitePoint[2] += all[i].b;
	}
	float top = 0;
	top = MAX(whitePoint[0], top);
	top = MAX(whitePoint[1], top);
	top = MAX(whitePoint[2], top);
	whitePoint[0] = top / whitePoint[0];
	whitePoint[1] = top / whitePoint[1];
	whitePoint[2] = top / whitePoint[2];
	return whitePoint;
}

float getMaximumDistance(ofVec2f& position, vector<ofVec2f*> positions) {
	float maximumDistance = 0;
	for(int i = 0; i < positions.size(); i++) {
		float distance = position.squareDistance(*positions[i]);
		if(distance > maximumDistance) {
			maximumDistance = distance;
		}
	}
	return sqrt(maximumDistance);
}

float getMinimumDistance(ofVec2f& position, vector<ofVec2f*> positions) {
	float minimumDistance = 0;
	for(int i = 0; i < positions.size(); i++) {
		float distance = position.squareDistance(*positions[i]);
		if(i == 0 || distance < minimumDistance) {
			minimumDistance = distance;
		}
	}
	return sqrt(minimumDistance);
}


uint64_t startTime;
void startTimer() {
    startTime = ofGetElapsedTimeMillis();
}
uint64_t checkTimer() {
    return ofGetElapsedTimeMillis() - startTime;
}

void loadMetadata(vector<FaceTrackerData>& data) {
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
                data.push_back(curData);
                total++;
            } else {
                ofLogWarning() << "couldn't find " << curData.getImageFilename();
//				ofLogWarning() << "removing metadata " << metadataFilename;
//				ofFile::removeFile(metadataFilename);
            }
        }
    }
    ofLog() << "done loading " << allDates.size() << " days, " << total << " faces in " << checkTimer() << "ms.";
}

void loadMetadata(BinnedData<FaceTrackerData>& binned) {
    vector<FaceTrackerData> data;
    loadMetadata(data);
    startTimer();
    for(FaceTrackerData& item : data) {
        binned.add(item.position, item);
    }
    ofLog() << "done binning " << data.size() << " faces in " << checkTimer() << "ms.";
}
