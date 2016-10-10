#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	BinnedData<ofVec2f> data;
	ofMesh historyMesh, dataMesh, neighborsMesh;
	
	void setup() {
		useSharedData();
		data.setup(ofGetWidth(), ofGetHeight(), 8);
		historyMesh.setMode(OF_PRIMITIVE_POINTS);
		dataMesh.setMode(OF_PRIMITIVE_POINTS);
		neighborsMesh.setMode(OF_PRIMITIVE_POINTS);
	}
	void update() {
		ofVec2f cur(mouseX, mouseY);
		historyMesh.addVertex(cur);
		vector<ofVec2f*> neighbors = data.getNeighborsRatio(cur, .1);
		neighborsMesh.clear();
		for(int i = 0; i < neighbors.size(); i++) {
			neighborsMesh.addVertex(*neighbors[i]);
		}
		float minimumDistance = getMinimumDistance(cur, neighbors);
		if(neighbors.size() == 0 || minimumDistance > 16) {
			data.add(cur, cur);
			dataMesh.addVertex(cur);
		}
	}
	void draw() {
		ofBackground(0);
		glPointSize(2);
		ofSetColor(32);
		historyMesh.draw();
		ofSetColor(255);
		dataMesh.draw();
		ofSetColor(255, 0, 0);
		neighborsMesh.draw();
		for(int i = 0; i < neighborsMesh.getNumVertices(); i++) {
			ofDrawLine(neighborsMesh.getVertex(i), ofVec2f(mouseX, mouseY));
		}
		ofDrawBitmapStringHighlight(ofToString(neighborsMesh.getNumVertices()), mouseX, mouseY);
		drawFramerate();
	}
};

int main() {
	ofSetupOpenGL(512, 512, OF_WINDOW);
	ofRunApp(new ofApp());
}
