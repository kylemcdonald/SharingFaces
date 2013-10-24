#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	BinnedData<ofVec2f> data;
	ofMesh history, neighborsMesh;
	
	void setup() {
		useSharedData();
		data.setup(ofGetWidth(), ofGetHeight(), 32, 32);
		history.setMode(OF_PRIMITIVE_POINTS);
		neighborsMesh.setMode(OF_PRIMITIVE_POINTS);
	}
	void update() {
		ofVec2f cur(mouseX, mouseY);
		history.addVertex(cur);
		vector<ofVec2f*> neighbors = data.getNeighbors(cur, 100);
		neighborsMesh.clear();
		for(int i = 0; i < neighbors.size(); i++) {
			neighborsMesh.addVertex(*neighbors[i]);
		}
//		if(neighbors.size() < 8) {
			data.add(cur, cur);
//		}
	}
	void draw() {
		ofBackground(0);
		glPointSize(4);
		ofSetColor(255);
		history.draw();
		ofSetColor(255, 0, 0);
		neighborsMesh.draw();
		drawFramerate();
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}