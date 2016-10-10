#define USE_BLACKMAGIC

#include "ofMain.h"
#include "ofxBlackMagic.h"

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
    for(int i = 0; i < 255; i++) {
        rmesh.addVertex(ofVec2f(i, 0));
        rmesh.addVertex(ofVec2f(i, r[i]));
        gmesh.addVertex(ofVec2f(i, 0));
        gmesh.addVertex(ofVec2f(i, g[i]));
        bmesh.addVertex(ofVec2f(i, 0));
        bmesh.addVertex(ofVec2f(i, b[i]));
    }
    ofPushMatrix();
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofScale(2, height / (samples / 255));
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
#ifdef USE_BLACKMAGIC
	ofxBlackMagic cam;
#else
    ofVideoGrabber cam;
#endif
	ofImage clipping;
    bool rotate;
    bool toggleGrayscale;
	
	void setup() {
        ofBackground(0);
#ifdef USE_BLACKMAGIC
        cam.setup(1920, 1080, 25);
        clipping.allocate(1920, 1080, OF_IMAGE_COLOR_ALPHA);
#else
        cam.initGrabber(1280, 720);
        clipping.allocate(1280, 720, OF_IMAGE_COLOR_ALPHA);
#endif
        rotate = true;
        toggleGrayscale = false;
	}
	void exit() {
		cam.close();
	}
	void update() {
#ifdef USE_BLACKMAGIC
        ofPixels& pix = cam.getColorPixels();
        if(cam.update()) {
#else
        cam.update();
        ofPixels& pix = cam.getPixels();
        if(cam.isFrameNew()) {
#endif
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
        }
        if(ofGetHeight() != 1920) {
            float scale = (float) ofGetWidth() / cam.getWidth();
            if(rotate) {
                scale = (float) ofGetHeight() / cam.getWidth();
            }
            ofScale(scale, scale);
        }
        ofTranslate(-cam.getWidth() / 2, -cam.getHeight() / 2);
#ifdef USE_BLACKMAGIC
        if(toggleGrayscale && ofGetElapsedTimeMillis() % 500 > 250) {
            cam.drawGray();
        } else {
            cam.drawColor();
        }
#else
        cam.draw(0, 0);
#endif 
        clipping.draw(0, 0);
        ofPopMatrix();
#ifdef USE_BLACKMAGIC
        drawHistogram(cam.getColorPixels(), mouseY);
#else
        drawHistogram(cam.getPixels(), mouseY);
#endif
	}
	void keyPressed(int key) {
		if(key == 'f') {
			ofToggleFullscreen();
		}
        if(key == '\t') {
            rotate = !rotate;
        }
        if(key == 'g') {
            toggleGrayscale = !toggleGrayscale;
        }
	}
};

int main() {
	ofSetupOpenGL(1080, 1920, OF_FULLSCREEN);
	ofRunApp(new ofApp());
}
