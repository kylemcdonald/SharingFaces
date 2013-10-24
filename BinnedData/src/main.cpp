#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	BinnedData<ofVec2f> data;
	ofMesh history, neighborsMesh;
	
	void setup() {
		useSharedData();
		data.setup(ofGetWidth(), ofGetHeight(), 8);
		history.setMode(OF_PRIMITIVE_POINTS);
		neighborsMesh.setMode(OF_PRIMITIVE_POINTS);
		for(int i = 0; i < 100000; i++) {
			float r = 256 * pow(ofRandom(1), 2), t = ofRandom(TWO_PI);
			ofVec2f cur(256 + sin(t) * r, 256 + cos(t) * r);
			history.addVertex(cur);
			data.add(cur, cur);
		}
	}
	void update() {
		ofVec2f cur(mouseX, mouseY);
//		history.addVertex(cur);
		vector<ofVec2f*> neighbors = data.getNeighborsRatio(cur, .01);
		neighborsMesh.clear();
		for(int i = 0; i < neighbors.size(); i++) {
			neighborsMesh.addVertex(*neighbors[i]);
		}
//		if(neighbors.size() < 8) {
//			data.add(cur, cur);
//		}
	}
	void draw() {
		ofBackground(0);
		glPointSize(2);
		ofSetColor(255);
		history.draw();
		ofSetColor(255, 0, 0);
		neighborsMesh.draw();
		for(int i = 0; i < neighborsMesh.getNumVertices(); i++) {
			ofLine(neighborsMesh.getVertex(i), ofVec2f(mouseX, mouseY));
		}
		ofDrawBitmapStringHighlight(ofToString(neighborsMesh.getNumVertices()), mouseX, mouseY);
		drawFramerate();
	}
};

int main() {
	ofSetupOpenGL(512, 512, OF_WINDOW);
	ofRunApp(new ofApp());
}