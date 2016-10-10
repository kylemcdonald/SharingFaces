#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	MultiThreadedImageSaver imageSaver;
	ofFbo fbo;
	ofPixels fboPixels;
	int currentFrame = 0;
	void setup() {
		ofSetLogLevel(OF_LOG_VERBOSE);
		fbo.allocate(ofGetWidth(), ofGetHeight());
	}
	void exit() {
		imageSaver.exit();
	}
	void draw() {
		fbo.begin();
		ofClear(0, 255);
		ofCircle(mouseX, mouseY, 100, 100);
		drawFramerate();
		ofDrawBitmapStringHighlight(ofToString(imageSaver.getQueueSize()), 10, 40);
		ofDrawBitmapStringHighlight(ofToString(currentFrame), 10, 60);
		fbo.end();
		fbo.draw(0, 0);
		fbo.readToPixels(fboPixels);
		fboPixels.setImageType(OF_IMAGE_COLOR);
		imageSaver.saveImage(fboPixels, ofToString(currentFrame) + ".jpg");
		currentFrame++;
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}
