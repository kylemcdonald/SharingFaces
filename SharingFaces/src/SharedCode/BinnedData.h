#pragma once

//#include "ofMain.h"

template <class T>
class BinnedData {
protected:
	int xBins, yBins;
	int binSize;
	int totalElements;
	vector< vector< vector<T> > > data;
	void bin(const ofVec2f& position, int& x, int& y) const {
		x = ofClamp(position.x / binSize, 0, xBins - 1);
		y = ofClamp(position.y / binSize, 0, yBins - 1);
	}
	
	ofMesh dataMesh, binMesh;
	
public:
	BinnedData()
	:totalElements(0) {
		
	}
	void setup(int width, int height, int binSize) {
		this->binSize = binSize;
		this->xBins = 1 + width / binSize, this->yBins = 1 + height / binSize;
		data = vector< vector< vector<T> > >(yBins, vector< vector<T> >(xBins, vector<T>()));
        dataMesh.setMode(OF_PRIMITIVE_POINTS);
        binMesh.setMode(OF_PRIMITIVE_LINES);
	}
	void add(const ofVec2f& position, T& element) {
		int x, y;
		bin(position, x, y);
        dataMesh.addVertex((ofVec3f) position);
		if(data[y][x].empty()) {
			int w = x * binSize, e = (x + 1) * binSize;
			int n = y * binSize, s = (y + 1) * binSize;
			binMesh.addVertex((ofVec3f) ofVec2f(w, n));
			binMesh.addVertex((ofVec3f) ofVec2f(e, n));
			binMesh.addVertex((ofVec3f) ofVec2f(e, n));
			binMesh.addVertex((ofVec3f) ofVec2f(e, s));
			binMesh.addVertex((ofVec3f) ofVec2f(e, s));
			binMesh.addVertex((ofVec3f) ofVec2f(w, s));
			binMesh.addVertex((ofVec3f) ofVec2f(w, s));
			binMesh.addVertex((ofVec3f) ofVec2f(w, n));
		}
		data[y][x].push_back(element);
		totalElements++;
	}
	void drawBins() {
        ofPushMatrix();
        ofTranslate(0.5, 0.5);
		ofPushStyle();
        ofSetColor(0,0,255);
		ofNoFill();
		ofSetLineWidth(1);
		binMesh.draw();
		ofPopStyle();
        ofPopMatrix();
	}
	void drawData() {
        ofPushStyle();
        ofSetColor(255,0,0);
		dataMesh.draw();
        ofPopStyle();
	}
    void drawCurrentPosition(const ofVec2f& pos) {
        ofPushStyle();
        ofSetColor(186,255,148);
        ofDrawCircle(pos.x, pos.y, 5);
        ofPopStyle();
    }
    void drawNearestPosition(const ofVec2f& pos) {
        ofPushStyle();
        ofSetColor(255,148,255);
        ofDrawCircle(pos.x, pos.y, 5);
        ofPopStyle();
    }
	vector<T*> getNeighborsRadius(const ofVec2f& position, float radius) {
		vector<T*> neighbors;
		int n, s, w, e;
		bin(position - ofVec2f(radius, radius), w, n);
		bin(position + ofVec2f(radius, radius), e, s);
		float squaredRadius = radius * radius;
		for(int y = n; y <= s; y++) {
			for(int x = w; x <= e; x++) {
				vector<T>& cur = data[y][x];
				for(int i = 0; i < cur.size(); i++) {
					float distance = cur[i].distanceSquared(position);
					if(distance < squaredRadius) {
						neighbors.push_back(&cur[i]);
					}
				}
			}
		}
		return neighbors;
	}
	vector<T*> getNeighborsCount(const ofVec2f& position, int count) {
		vector<T*> neighbors;
		if(count >= totalElements) {
			for(int y = 0; y < yBins; y++) {
				for(int x = 0; x < xBins; x++) {
					vector<T>& cur = data[y][x];
					for(int j = 0; j < cur.size(); j++) {
						neighbors.push_back(&cur[j]);
					}
				}
			}
		} else {
			for(int i = 0; neighbors.size() < count; i++) {
				int x, y;
				bin(position, x, y);
				int n = y - i, s = y + i, w = x - i, e = x + i;
				for(int y = n; y <= s; y++) {
					for(int x = w; x <= e; x++) {
						if(y == n || y == s || x == w || x == e) {
							if(x >= 0 && x < xBins && y >= 0 && y < yBins) {
								vector<T>& cur = data[y][x];
								for(int j = 0; j < cur.size(); j++) {
									neighbors.push_back(&cur[j]);
								}
							}
						}
					}
				}
			}
		}
		return neighbors;
	}
	vector<T*> getNeighborsRatio(const ofVec2f& position, float ratio) {
		return getNeighborsCount(position, ratio * totalElements);
	}
};
