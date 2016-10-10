#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	BinnedData<FaceTrackerData> data;
	
	void setup() {
		useSharedData();
        data.setup(1080, 1920, 10);
        loadMetadata(data);
        ofSetBackgroundAuto(false);
	}
	void update() {
	}
	void draw() {
		ofSetColor(255);
        ofVec2f position(mouseX, mouseY);
        vector<FaceTrackerData*> results = data.getNeighborsCount(position, 1);
        for(FaceTrackerData* cur : results) {
            ofImage img(cur->getImageFilename());
            int r = 200;
            int ox = cur->position.x - r/2;
            int oy = cur->position.y - r/2;
            img.drawSubsection(ox, oy, r, r, ox, oy);
        }
	}
	void keyPressed(int key) {
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}
