#pragma once

#include "ofMain.h"

template <class T>
class BinnedData {
protected:
	int xBins, yBins;
	int xBinSize, yBinSize;
	vector< vector< vector<T> > > data;
	void bin(const ofVec2f& position, int& x, int& y) const {
		x = ofClamp(position.x / xBinSize, 0, xBins - 1);
		y = ofClamp(position.y / yBinSize, 0, yBins - 1);
	}
	
public:
	void setup(int width, int height, int xBinSize, int yBinSize) {
		this->xBinSize = xBinSize, this->yBinSize = yBinSize;
		this->xBins = 1 + width / xBinSize, this->yBins = 1 + height / yBinSize;
		data = vector< vector< vector<T> > >(yBins, vector< vector<T> >(xBins, vector<T>()));
	}
	void add(const ofVec2f& position, T& element) {
		int x, y;
		bin(position, x, y);
		data[y][x].push_back(element);
	}
	vector<T*> getNeighbors(const ofVec2f& position, float radius) {
		vector<T*> neighbors;
		int x, y;
		bin(position, x, y);
		vector<T>& cur = data[y][x];
		for(int i = 0; i < cur.size(); i++) {
			neighbors.push_back(&cur[i]);
		}
		return neighbors;
	}
};