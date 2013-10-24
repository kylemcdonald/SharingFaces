#include "ofMain.h"

#include "ofxCv.h"
#include "ofxFaceTracker.h"

class ofApp : public ofBaseApp {
public:
	ofVideoGrabber cam;
	ofxFaceTracker faceTracker;
	
	void setup() {
		cam.initGrabber(640, 480);
	}
	void update() {

	}
	void draw() {
		
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}