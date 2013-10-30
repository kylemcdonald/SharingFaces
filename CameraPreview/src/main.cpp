#include "ofMain.h"
#include "ofxBlackMagic.h"

class ofApp : public ofBaseApp {
public:
	ofxBlackMagic cam;
	ofImage img;
	
	void setup() {
		cam.setup(1920, 1080, 30);
	}
	void exit() {
		cam.close();
	}
	void update() {
		cam.update();
	}
	void draw() {
		ofRotate(-90);
		ofTranslate(-ofGetHeight(), 0);
		cam.drawColor();
	}
	void keyPressed(int key) {
		if(key == 'f') {
			ofToggleFullscreen();
		}
	}
};

int main() {
	ofSetupOpenGL(1080, 1920, OF_FULLSCREEN);
	ofRunApp(new ofApp());
}