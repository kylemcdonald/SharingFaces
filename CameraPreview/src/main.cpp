#include "ofMain.h"

void drawHistogram(const ofPixels& pix, float height = 128, int skip = 16) {
    vector<float> r(256), g(256), b(256);
    const unsigned char* data = pix.getData();
    int n = pix.size();
    int samples = 0;
    for(int i = 0; i < n; i += 3*skip) {
        r[data[i++]]++;
        g[data[i++]]++;
        b[data[i++]]++;
        samples++;
    }
    ofMesh rmesh, gmesh, bmesh;
    rmesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    gmesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    bmesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    int peak = 0;
    for(int i = 0; i < 255; i++) {
        rmesh.addVertex(ofVec3f(i, 0));
        rmesh.addVertex(ofVec3f(i, r[i]));
        gmesh.addVertex(ofVec3f(i, 0));
        gmesh.addVertex(ofVec3f(i, g[i]));
        bmesh.addVertex(ofVec3f(i, 0));
        bmesh.addVertex(ofVec3f(i, b[i]));
        peak = MAX(peak, r[i]);
        peak = MAX(peak, g[i]);
        peak = MAX(peak, b[i]);
    }
    ofPushMatrix();
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofScale(2, height / peak);
    ofTranslate(.5, 0);
    ofSetColor(255, 0, 0);
    rmesh.draw();
    ofSetColor(0, 255, 0);
    gmesh.draw();
    ofSetColor(0, 0, 255);
    bmesh.draw();
    ofPopStyle();
    ofPopMatrix();
}

class ofApp : public ofBaseApp {
public:
    ofVideoGrabber cam;
	ofImage clipping;
    bool fullscreen = true;
    bool rotate = false;
    bool toggleGrayscale = false;
    bool installationMode = false;
    int camWidth = 1280, camHeight = 720;
	
	void setup() {
        installationMode = ofGetScreenWidth() < ofGetScreenHeight();
        if(installationMode) {
            rotate = true;
            camWidth = 1920, camHeight = 1080;
        }
        
        ofBackground(0);
        cam.initGrabber(camWidth, camHeight);
        clipping.allocate(camWidth, camHeight, OF_IMAGE_COLOR_ALPHA);
        toggleGrayscale = false;
	}
	void exit() {
		cam.close();
	}
	void update() {
        cam.update();
        ofPixels& pix = cam.getPixels();
        if(cam.isFrameNew()) {
            int skip = 2;
            int range = mouseX / 25;
            for(int y = 0; y < cam.getHeight(); y += skip) {
                for(int x = 0; x < cam.getWidth(); x += skip) {
                    ofColor cur = pix.getColor(x, y);
                    ofColor result(0, 0, 0, 0);
                    if(cur.r < range || cur.r > 255-range) {
                        result.r = 255;
                        result.a = 255;
                    }
                    if(cur.g < range || cur.g > 255-range) {
                        result.g = 255;
                        result.a = 255;
                    }
                    if(cur.b < range || cur.b > 255-range) {
                        result.b = 255;
                        result.a = 255;
                    }
                    clipping.setColor(x, y, result);
                }
            }
            clipping.update();
        }   
	}
    void draw() {
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        if(rotate) {
            ofRotate(90);
        } else {
            float scale = (float) ofGetWidth() / cam.getWidth();
            if(rotate) {
                scale = (float) ofGetHeight() / cam.getWidth();
            }
            ofScale(scale, scale);
        }
        ofTranslate(-cam.getWidth() / 2, -cam.getHeight() / 2);
        cam.draw(0, 0);
        clipping.draw(0, 0);
        ofPopMatrix();
        drawHistogram(cam.getPixels(), mouseY);
	}
    void updateWindowShape() {
        if(!fullscreen) {
            if(rotate) {
                ofSetWindowShape(camHeight, camWidth);
            } else {
                ofSetWindowShape(camWidth, camHeight);
            }
        }
    }
	void keyPressed(int key) {
		if(key == 'f') {
            fullscreen = !fullscreen;
            ofSetFullscreen(fullscreen);
            updateWindowShape();
		}
        if(key == '\t') {
            rotate = !rotate;
            updateWindowShape();
        }
        if(key == 'g') {
            toggleGrayscale = !toggleGrayscale;
        }
	}
};

int main() {
	ofSetupOpenGL(1080, 1080, OF_FULLSCREEN);
	ofRunApp(new ofApp());
}
