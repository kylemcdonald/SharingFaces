#include "ofMain.h"
#include "ofxBlackmagicGrabber.h"
#include "ImageSaver.h"

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
    ofSetColor(255);
    ofDrawLine(256, 0, 256, peak);
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
    ofxBlackMagicGrabber blackmagicGrabber;
    ofVideoGrabber videoGrabber;
    ofVideoGrabber* cam;
    
    MultiThreadedImageSaver imageSaver;
    
    ofImage clipping;
    bool toggleGrayscale = false;
    ofJson config;
    string device;
    bool recording = false;
    int frameCount = 0;
    
    void setup() {
        ofSetVerticalSync(true);
        ofBackground(0);
        config = ofLoadJson("../../../SharedData/shared/config.json");
        float camWidth = config["camera"]["width"];
        float camHeight = config["camera"]["height"];
        float camFrameRate = config["camera"]["framerate"];
        device = config["camera"]["device"];
        if (device == "blackmagic") {
            cam = &blackmagicGrabber;
        } else {
            cam = &videoGrabber;
        }
        cam->setDesiredFrameRate(camFrameRate);
        cam->setup(camWidth, camHeight);
        clipping.allocate(camWidth, camHeight, OF_IMAGE_COLOR_ALPHA);
        toggleGrayscale = false;
        updateWindowShape();
    }
    void exit() {
        imageSaver.exit();
        cam->close();
    }
    void update() {
        cam->update();
        if(cam->isFrameNew()) {
            ofPixels& pix = cam->getPixels();
            int skip = 2;
            int range = mouseX / 25;
            for(int y = 0; y < pix.getHeight(); y += skip) {
                for(int x = 0; x < pix.getWidth(); x += skip) {
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
            
            if(recording) {
                string fn = "images/" + ofToString(frameCount, 6, '0') + ".jpg";
                imageSaver.saveImage(pix, fn);
                frameCount++;
            }
        }
    }
    void draw() {
        ofPushMatrix();
        float screenWidth = config["screen"]["width"];
        float screenHeight = config["screen"]["height"];
        float camWidth = config["camera"]["width"];
        float camHeight = config["camera"]["height"];
        ofTranslate(screenWidth / 2, screenHeight / 2);
        ofRotateZDeg(config["camera"]["rotate"]);
        ofTranslate(-camWidth / 2, -camHeight / 2);
        if(cam->isInitialized()) {
            cam->draw(0,0);
        }
        clipping.draw(0, 0);
        ofPopMatrix();
        if(cam->isInitialized()) {
            drawHistogram(cam->getPixels(), mouseY);
        }
    }
    void updateWindowShape() {
        ofSetWindowShape(config["screen"]["width"], config["screen"]["height"]);
        ofSetWindowPosition(config["screen"]["x"], config["screen"]["y"]);
        ofSetFullscreen(config["screen"]["fullscreen"]);
    }
    void keyPressed(int key) {
        if(key == 'r') {
            recording = !recording;
        }
        if(key == 'f') {
            config["screen"]["fullscreen"] = !config["screen"]["fullscreen"];
            updateWindowShape();
        }
        if(key == '\t') {
            config["camera"]["rotate"] = config["camera"]["rotate"] == 0 ? 90 : 0;
            updateWindowShape();
        }
        if(key == 'g') {
            toggleGrayscale = !toggleGrayscale;
        }
    }
};

int main() {
    ofSetupOpenGL(1080, 1080, OF_WINDOW);
    ofRunApp(new ofApp());
}
