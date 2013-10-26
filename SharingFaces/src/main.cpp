#include "ofMain.h"

#include "SharingFacesUtils.h"

//#define INSTALL

class ofApp : public ofBaseApp {
public:
#ifdef INSTALL
	static const int camWidth = 1920, camHeight = 1080;
	ofxBlackmagicGrabber cam;
#else
	static const int camWidth = 1280, camHeight = 720;
	ofVideoGrabber cam;
#endif
	ofShader shader;
	ofxFaceTracker tracker;
	BinnedData<FaceTrackerData> data;
	FaceCompare faceCompare;
	ThreadedImageSaver imageSaver;
	
	bool useBlackMagic;
	bool rotate;
	int binSize;
	float neighborRadius;
	int neighborCount;
	ofImage rotated;
	
	FaceTrackerData nearestData;
	string lastLabel;
	ofImage similar;
	
	Hysteresis presence;
	vector< pair<ofVec2f, FaceTrackerData> > currentData;
	
	void setup() {
#ifndef INSTALL
		useSharedData();
#endif
		loadSettings();
		tracker.setup();
#ifdef INSTALL
		cam.setVideoMode(bmdModeHD1080p30);
		cam.setDeinterlace(false);
		cam.initGrabber(camWidth, camHeight);
#else
		cam.initGrabber(camWidth, camHeight, false);
#endif
		if(rotate) {
			data.setup(camHeight, camWidth, binSize);
		} else {
			data.setup(camWidth, camHeight, binSize);
		}
		loadMetadata(data);
		presence.setDelay(0, 10);
		shader.load("shaders/colorbalance.vs", "shaders/colorbalance.fs");
		ofDisableAntiAliasing();
		ofSetLogLevel(OF_LOG_VERBOSE);
	}
	void loadSettings() {
#ifdef INSTALL
		rotate = true;
#else
		rotate = false;
#endif
		binSize = 10;
		neighborRadius = 20;
		neighborCount = 100;
	}
	void update() {
		cam.update();
		if(cam.isFrameNew()) {
			// next two could be replaced with one line
			ofxCv::rotate90(cam, rotated, rotate ? 270 : 0);
			ofxCv:flip(rotated, rotated, 1);
			Mat rotatedMat = toCv(rotated);
			tracker.update(rotatedMat);
			if(tracker.getFound()) {
				ofVec2f position = tracker.getPosition();
				// should be count capped at a maximum radius
				// maximum count is to ensure we don't spend too much time searching
				// maximum radius is to ensure sparse areas don't search far away
				vector<FaceTrackerData*> neighbors = data.getNeighborsCount(position, neighborCount);
				FaceTrackerData curData;
				curData.load(tracker);
				if(!neighbors.empty()) {
					nearestData = *faceCompare.nearest(curData, neighbors);
					if(nearestData.label != lastLabel) {
						similar.loadImage("images/" + nearestData.label + ".jpg");
					}
					lastLabel = nearestData.label;
				}
				if(faceCompare.different(curData, neighbors)) {
					saveFace(curData, rotated);
					currentData.push_back(pair<ofVec2f, FaceTrackerData>(position, curData));
				}
			}
			presence.update(tracker.getFound());
			if(presence.wasUntriggered()) {
				for(int i = 0; i < currentData.size(); i++) {
					data.add(currentData[i].first, currentData[i].second);
				}
				currentData.clear();
			}
		}
	}
	void draw() {
		ofSetColor(255);
		if(similar.isAllocated()) {
			shader.begin();
			shader.setUniformTexture("tex", similar, 0);
			similar.draw(0, 0);
			shader.end();
		}
		tracker.draw();
		ofNoFill();
		ofCircle(tracker.getPosition(), 10);
		
		data.draw();
		
		ofPushStyle();
		ofSetColor(255, 0, 0);
		nearestData.draw();
		ofPopStyle();
		
#ifndef INSTALL
		drawFramerate();
#endif
	}
	void keyPressed(int key) {
		if(key == 'f') {
			ofToggleFullscreen();
		}
	}
	
	void saveFace(FaceTrackerData& data, ofImage& img) {
		string basePath = ofGetTimestampString("%Y.%m.%d/%H.%M.%S.%i");
		data.save("metadata/" + basePath + ".face");
		imageSaver.saveImage(img.getPixelsRef(), "images/" + basePath + ".jpg");
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}