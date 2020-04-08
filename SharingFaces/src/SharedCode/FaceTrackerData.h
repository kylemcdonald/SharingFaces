/*
 this class is only for serializing face tracking data. any analysis or
 processing should be done in anohter class or file.
 */

#pragma once
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
    const static int numExpressions = 52;
	string label;
	ofVec2f position;
	float scale;
	ofVec3f orientation;
    vector<float> expressions;
	
    void load(const ARKitFaceTracker& tracker) {
        position = tracker.getPosition();
        scale = tracker.getScale();
        orientation = tracker.getOrientation();
        expressions = tracker.getExpressions();
    }
    
	void load(const string& filename) {
		setLabel(filename);
		ofFile file;
		file.open(filename, ofFile::ReadOnly, true);
        loadRaw(file, position);
        loadRaw(file, orientation);
        loadRaw(file, scale);
        loadRaw(file, expressions, numExpressions);
		file.close();
	}
    
	void save(const string& filename) {
		setLabel(filename);
		ofFile file;
		file.open(filename, ofFile::WriteOnly, true);
        saveRaw(file, position);
        saveRaw(file, orientation);
        saveRaw(file, scale);
        saveRaw(file, expressions);
		file.close();
	}
    
	float distanceSquared(const ofVec2f& position) const {
		return this->position.squareDistance(position);
	}
    
	void draw() {
//        ofMesh mesh;
//        mesh.setMode(OF_PRIMITIVE_POINTS);
//        for(int i = 0; i < imagePoints.size(); i++) {
//            mesh.addVertex(ofVec3f(imagePoints[i]));
//        }
//        mesh.draw();
        
        // draw face overlay of some kind, if you want
	}
    
	string getImageFilename() const {
		return "images/" + label + ".jpg";
	}
};
