#include "ofMain.h"

#include "SharingFacesUtils.h"

class ofApp : public ofBaseApp {
public:
	static const int camWidth = 1280, camHeight = 720;
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	BinnedData<FaceTrackerData> data;
	FaceCompare faceCompare;
	
	bool rotate;
	int binSize;
	float neighborRadius = 100;
	int neighborCount;
	ofImage rotated;
	
	ofMesh historyMesh, dataMesh;
	
	void setup() {
		useSharedData();
		ofSetLogLevel(OF_LOG_VERBOSE);
		loadSettings();
		tracker.setup();
		cam.initGrabber(camWidth, camHeight, false);
		historyMesh.setMode(OF_PRIMITIVE_POINTS);
		dataMesh.setMode(OF_PRIMITIVE_POINTS);
		if(rotate) {
			data.setup(camHeight, camWidth, binSize);
		} else {
			data.setup(camWidth, camHeight, binSize);
		}
	}
	void loadSettings() {
		rotate = false;
		binSize = 10;
		neighborCount = 100;
	}
	void update() {
		cam.update();
		if(cam.isFrameNew()) {
			ofxCv::rotate90(cam, rotated, rotate ? 90 : 0);
			rotated.update();
			Mat rotatedMat = toCv(rotated);
			tracker.update(rotatedMat);
			if(tracker.getFound()) {
				ofVec2f position = tracker.getPosition();
				historyMesh.addVertex(position);
				// this should be radius but capped at a maximum count
				// or count capped at a maximum radius, whicever is more efficient
				vector<FaceTrackerData*> neighbors = data.getNeighborsRadius(position, neighborRadius);
				FaceTrackerData curData;
				curData.load(tracker);
				if(faceCompare.different(curData, neighbors)) {
					dataMesh.addVertex(position);
					data.add(position, curData);
				}
			}
		}
	}
	void draw() {
		ofSetColor(255);
		if(rotated.isAllocated()) {
			rotated.draw(0, 0);
		}
		tracker.draw();
		ofNoFill();
		ofCircle(tracker.getPosition(), 10);
		
		ofPushStyle();
		glPointSize(4);
		ofSetColor(32);
		historyMesh.draw();
		ofSetColor(255);
		dataMesh.draw();
		ofPopStyle();
		
		string info =
		ofToString(tracker.getPosition().x) + "x" + ofToString(tracker.getPosition().y) + "\n" +
		ofToString(tracker.getOrientation().x) + ", " + ofToString(tracker.getOrientation().y) + ", " + ofToString(tracker.getOrientation().z)  + "\n" +
		ofToString(tracker.getScale());
//		ofDrawBitmapStringHighlight(info, tracker.getPosition());
		drawFramerate();
	}
	void keyPressed(int key) {
	}
};

int main() {
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}