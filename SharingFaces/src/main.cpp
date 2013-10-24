#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	
	bool rotate;
	ofImage rotated;
	
	ofMesh history;
	
	void setup() {
		useSharedData();
		ofSetLogLevel(OF_LOG_VERBOSE);
		loadSettings();
		tracker.setup();
		cam.initGrabber(1280, 720, false);
		history.setMode(OF_PRIMITIVE_POINTS);
	}
	void loadSettings() {
		rotate = false;
	}
	void update() {
		cam.update();
		if(cam.isFrameNew()) {
			ofxCv::rotate90(cam, rotated, rotate ? 90 : 0);
			rotated.update();
			Mat rotatedMat = toCv(rotated);
			tracker.update(rotatedMat);
			if(tracker.getFound()) {
				history.addVertex(tracker.getPosition());
			}
		}
	}
	void draw() {
		ofSetColor(255);
		if(rotated.isAllocated()) {
			rotated.draw(0, 0);
		}
		tracker.draw();
		ofNoFill();
		ofCircle(tracker.getPosition(), 10);
		glPointSize(4);
		history.draw();
		drawFramerate();
	}
	void keyPressed(int key) {
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}