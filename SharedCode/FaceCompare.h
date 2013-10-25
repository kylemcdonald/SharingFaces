#pragma once

#include "ofxFaceTracker.h"
#include "FaceTrackerData.h"

class FaceCompare {
protected:
	// turn the scaling factors up to get more quantized data
	float scalePosition, scaleScale, scaleOrientation, scaleExpression;
public:
	FaceCompare()
	:scalePosition(.1)
	,scaleScale(2.25)
	,scaleOrientation(20)
	,scaleExpression(2) {
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
	static float closestScale(const FaceTrackerData& target, const vector<FaceTrackerData*>& data) {
		float closest = 0;
		for(int i = 0; i < data.size(); i++) {
			float distance = abs(target.scale - data[i]->scale);
			if(i == 0 || distance < closest) {
				closest = distance;
			}
		}
		return closest;
	}
	static float closestOrientation(const FaceTrackerData& target, const vector<FaceTrackerData*>& data) {
		float closest = 0;
		for(int i = 0; i < data.size(); i++) {
			float distance = 0;
			distance = MAX(distance, abs(target.orientation.x - data[i]->orientation.x));
			distance = MAX(distance, abs(target.orientation.y - data[i]->orientation.y));
			distance = MAX(distance, abs(target.orientation.z - data[i]->orientation.z));
			if(i == 0 || distance < closest) {
				closest = distance;
			}
		}
		return closest;
	}
	static float closestExpression(const FaceTrackerData& target, const vector<FaceTrackerData*>& data) {
		float closest = 0;
		const vector<ofVec3f>& a = target.objectPoints;
		for(int i = 0; i < data.size(); i++) {
			float distance = 0;
			const vector<ofVec3f>& b = data[i]->objectPoints;
			for(int j = 0; j < a.size(); j++) {
				distance = MAX(distance, a[j].distanceSquared(b[j]));
			}
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
		return
		closestPosition(target, data) * scalePosition > 1 ||
		closestScale(target, data) * scaleScale > 1 ||
		closestOrientation(target, data) * scaleOrientation > 1 ||
		closestExpression(target, data) * scaleExpression > 1;
	}
	float distance(const FaceTrackerData& a, const FaceTrackerData& b) {
		float distancePosition = a.position.distance(b.position);
		float distanceScale =  abs(a.scale - b.scale);
		float distanceOrientation = a.orientation.distance(b.orientation); // approximation
		float distanceExpression = 0;
		const vector<ofVec3f>& aobj = a.objectPoints, bobj = b.objectPoints;
		for(int i = 0; i < aobj.size(); i++) {
			distanceExpression += aobj[i].distanceSquared(bobj[i]);
		}
		distanceExpression = sqrt(distanceExpression);
		distancePosition *= scalePosition;
		distanceScale *= scaleScale;
		distanceOrientation *= scaleOrientation;
		distanceExpression *= scaleExpression;
		return sqrt(distancePosition * distancePosition +
					distanceScale * distanceScale +
					distanceOrientation * distanceOrientation +
					distanceExpression * distanceExpression);
	}
	FaceTrackerData* nearest(const FaceTrackerData& target, vector<FaceTrackerData*>& data) {
		float closest = 0;
		FaceTrackerData* closestData = NULL;
		for(int i = 0; i < data.size(); i++) {
			float distance = FaceCompare::distance(target, *data[i]);
			if(i == 0 || distance < closest) {
				closest = distance;
				closestData = data[i];
			}
		}
		return closestData;
	}
};