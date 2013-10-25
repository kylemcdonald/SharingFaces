/*
 this class is only for serializing face tracking data. any analysis or
 processing should be done in anohter class or file.
 */

#pragma once

#include "ofxFaceTracker.h"
#include "ofMain.h"
#include "RawUtils.h"

class FaceTrackerData {
protected:
	void setLabel(const string& filename) {
		string prefix = "metadata/";
		string postfix = ".face";
		label = filename.substr(prefix.length(), filename.length() - prefix.length() - postfix.length());
	}
public:
	const static int pointCount = 66;
	
	string label;
	
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
		setLabel(filename);
		ofFile file;
		file.open(filename, ofFile::ReadOnly, true);
		loadRaw(file, position);
		loadRaw(file, scale);
		loadRaw(file, orientation);
		loadRaw(file, objectPoints, pointCount);
		loadRaw(file, imagePoints, pointCount);
		file.close();
	}
	void save(const string& filename) {
		setLabel(filename);
		ofFile file;
		file.open(filename, ofFile::WriteOnly, true);
		saveRaw(file, position);
		saveRaw(file, scale);
		saveRaw(file, orientation);
		saveRaw(file, objectPoints);
		saveRaw(file, imagePoints);
		file.close();
	}
	float distanceSquared(const ofVec2f& position) const {
		return this->position.distanceSquared(position);
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