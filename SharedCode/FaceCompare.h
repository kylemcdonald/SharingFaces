#pragma once

#include "ofxFaceTracker.h"
#include "FaceTrackerData.h"

class FaceCompare {
protected:
	float scalePosition, scaleScale, scaleOrientation, scaleExpression;
public:
	FaceCompare()
	:scalePosition(.1)
	,scaleScale(2.5)
	,scaleOrientation(11.5)
	,scaleExpression(1) {
	}
	static float closestPosition(const FaceTrackerData& target, const vector<FaceTrackerData*>& data) {
		float closest = 0;
		for(int i = 0; i < data.size(); i++) {
			float distance = data[i]->position.distanceSquared(target.position);
			if(i == 0 || distance < closest) {
				closest = distance;
			}
		}
		return sqrt(closest);
	}
	bool different(const FaceTrackerData& target, const vector<FaceTrackerData*>& data) {
		if(data.empty()) {
			return true;
		}
		return closestPosition(target, data) > (1 / scalePosition);
		// add extra checks here
	}
};