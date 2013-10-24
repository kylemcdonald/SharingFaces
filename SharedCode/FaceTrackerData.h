/*
 this class is only for serializing face tracking data. any analysis or
 processing should be done in anohter class or file.
 */

#pragma once

#include "ofxFaceTracker.h"
#include "ofMain.h"
#include "RawUtils.h"

class FaceTrackerData {
public:
	const static int pointCount = 66;
	
	ofVec2f position;
	float scale;
	ofVec3f orientation;
	vector<ofVec3f> objectPoints;
	vector<ofVec2f> imagePoints;
	
	void load(const ofxFaceTracker& tracker) {
		position = tracker.getPosition();
		scale = tracker.getScale();
		orientation = tracker.getOrientation();
		objectPoints = tracker.getObjectPoints();
		imagePoints = tracker.getImagePoints();
	}
	void load(const string& filename) {
		ifstream file;
		file.open(ofToDataPath(filename).c_str(), ios::in | ios::binary);
		if (file.is_open()) {
			loadRaw(file, position);
			loadRaw(file, scale);
			loadRaw(file, orientation);
			loadRaw(file, objectPoints, pointCount);
			loadRaw(file, imagePoints, pointCount);
		} else {
			ofLogError("FaceTrackerData") << "couldn't load data from " << filename;
		}
		file.close();
	}
	void save(const string& filename) {
		ofstream file;
		file.open(ofToDataPath(filename).c_str(), ios::out | ios::binary);
		if (file.is_open()) {
			saveRaw(file, position);
			saveRaw(file, scale);
			saveRaw(file, orientation);
			saveRaw(file, objectPoints);
			saveRaw(file, imagePoints);
		} else {
			ofLogError("FaceTrackerData") << "couldn't save data to " << filename;
		}
		file.close();
	}
	void draw() {
		ofMesh mesh;
		mesh.setMode(OF_PRIMITIVE_POINTS);
		for(int i = 0; i < imagePoints.size(); i++) {
			mesh.addVertex(imagePoints[i]);
		}
		mesh.draw();
	}
};