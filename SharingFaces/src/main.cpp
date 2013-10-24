#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	
	void setup() {
		useSharedData();
		tracker.setup();
		cam.initGrabber(1280, 720);
	}
	void update() {
		cam.update();
		if(cam.isFrameNew()) {
			Mat camMat = toCv(cam);
			tracker.update(camMat);
		}
	}
	void draw() {
		ofSetColor(255);
		cam.draw(0, 0);
		tracker.draw();
	}
	void keyPressed(int key) {
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}