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
		neighborRadius = 20;
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
				// should be count capped at a maximum radius
				// maximum count is to ensure we don't spend too much time searching
				// maximum radius is to ensure sparse areas don't search far away
				vector<FaceTrackerData*> neighbors = data.getNeighborsCount(position, neighborCount);
				FaceTrackerData curData;
				curData.load(tracker);
				if(faceCompare.different(curData, neighbors)) {
					dataMesh.addVertex(position);
					data.add(position, curData);
					saveFace(curData, rotated);
				} else {
					historyMesh.addVertex(position);
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
		ofDrawBitmapStringHighlight(ofToString(imageSaver.getActiveThreads()), 10, 40);
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