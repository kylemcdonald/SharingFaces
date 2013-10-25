#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	static const int camWidth = 1280, camHeight = 720;
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	BinnedData<FaceTrackerData> data;
	FaceCompare faceCompare;
	MultiThreadedImageSaver imageSaver;
	
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
		useSharedData();
		loadSettings();
		tracker.setup();
		cam.initGrabber(camWidth, camHeight, false);
		if(rotate) {
			data.setup(camHeight, camWidth, binSize);
		} else {
			data.setup(camWidth, camHeight, binSize);
		}
		loadMetadata(data);
		presence.setDelay(0, 10);
		ofSetLogLevel(OF_LOG_VERBOSE);
	}
	void loadSettings() {
		rotate = false;
		binSize = 10;
		neighborRadius = 20;
		neighborCount = 100;
	}
	void update() {
		cam.update();
		if(cam.isFrameNew()) {
			ofxCv::rotate90(cam, rotated, rotate ? 90 : 0);
			ofxCv:flip(rotated, rotated, 1);
			rotated.update();
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
		if(rotated.isAllocated()) {
			rotated.draw(0, 0);
		}
		if(similar.isAllocated()) {
			similar.draw(0, 0);
		}
		tracker.draw();
		ofNoFill();
		ofCircle(tracker.getPosition(), 10);
		
		data.draw();
		
		ofPushStyle();
		ofSetColor(255, 0, 0);
		nearestData.draw();
		ofPopStyle();
		
		drawFramerate();
	}
	void keyPressed(int key) {
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