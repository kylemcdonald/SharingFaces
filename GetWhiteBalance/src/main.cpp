#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	ofImage a, b;
	ofVec3f aw, bw;
	ofShader shader;
	void setup() {
		useSharedData();
		shader.load("shaders/colorbalance.vs", "shaders/colorbalance.fs");
		a.loadImage("a.jpg");
		b.loadImage("b.jpg");
	}
	void exit() {
	}
	void update() {
		aw = getWhitePoint(a);
		bw = getWhitePoint(b);
		if(ofGetMousePressed()) {
			aw.set(1, 1, 1);
			bw.set(1, 1, 1);
		}
	}
	void draw() {
		ofBackground(255);
		ofPushMatrix();
		ofScale(.5, .5);
		shader.begin();
		shader.setUniform3fv("whitePoint", (float*) &aw);
		shader.setUniformTexture("tex", a, 0);
		a.draw(0, 0);
		shader.setUniform3fv("whitePoint", (float*) &bw);
		shader.setUniformTexture("tex", b, 0);
		b.draw(1080, 0);
		shader.end();
		ofPopMatrix();
		ofDrawBitmapStringHighlight(ofToString((int) ofGetFrameRate()), 10, 20);
	}
};

int main() {
	ofSetupOpenGL(1080, 1920/2, OF_WINDOW);
	ofRunApp(new ofApp());
}
