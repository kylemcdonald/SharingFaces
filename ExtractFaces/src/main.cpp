#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	vector<FaceTrackerData> data;
    int offset = 0;
    
	void setup() {
		useSharedData();
        loadMetadata(data);
        ofBackground(0);
        ofSetBackgroundAuto(false);
        ofSetColor(255);
	}
	void update() {
	}
	void draw() {
        if(ofGetFrameNum() > 4) {
            for(int i = 0; i < 4; i++) {
                if(offset < data.size()) {
                    FaceTrackerData& cur = data[offset];
                    ofImage img(cur.getImageFilename());
                    int r = 50 * cur.scale;
                    int ox = cur.position.x - r/2;
                    int oy = cur.position.y - r/2;
                    
        //            img.drawSubsection(ox, oy, r, r, ox, oy);
                    
                    int s = 24;
                    int x = (offset * s) % ofGetWidth();
                    int y = (offset * s) / ofGetWidth();
                    y *= s;
                    img.drawSubsection(x, y, s, s, ox, oy, r, r);
                    
                }
                offset++;
            }
        }
	}
	void keyPressed(int key) {
	}
};

int main() {
	ofSetupOpenGL(1920, 1080, OF_FULLSCREEN);
	ofRunApp(new ofApp());
}
