#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	FaceTrackerData trackerDataSave, trackerDataLoad;
	
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
			trackerDataSave.load(tracker);
		}
	}
	void draw() {
		ofSetColor(255);
		cam.draw(0, 0);
		tracker.draw();
		glPointSize(4);
		ofSetColor(ofColor::red);
		trackerDataSave.draw();
		ofSetColor(ofColor::blue);
		trackerDataLoad.draw();
	}
	void keyPressed(int key) {
		if(key == 's') {
			trackerDataSave.save("metadata/out.face");
		}
		if(key == 'l') {
			trackerDataLoad.load("metadata/out.face");
		}
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}
