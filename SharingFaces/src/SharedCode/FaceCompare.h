#pragma once

//#include "ofxFaceTracker.h"
#include "FaceTrackerData.h"

class FaceCompare {
protected:
	// turn the scaling factors up to get more quantized data
    // ELLIOTT, 8/1/19: I took some measurements, and the average raw data we're getting back is as follows:
    // distancePosition: between 10 and 20
    // distanceScale: between 0.25 and 0.35
    // distanceOrientation: between 30 and 40
    // distanceExpression: between 1.25 and 1.5
    // I've set the scaling factors so that we average about 1 for each of them, with a slight bias for expression.
	float scalePosition, scaleScale, scaleOrientation, scaleExpression;
public:
	FaceCompare()
	:scalePosition(0.125)
	,scaleScale(2)
	,scaleOrientation(0.25)
	,scaleExpression(1.5) {
	}
	static float closestPosition(const FaceTrackerData& target, const vector<FaceTrackerData*>& data) {
		float closest = 0;
		for(int i = 0; i < data.size(); i++) {
			float distance = data[i]->position.squareDistance(target.position);
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
        const auto& a = target.expressions;
        for (int i = 0; i < data.size(); i++) {
            float distance = 0;
            const auto& b = data[i]->expressions;
            distance = vectors_euclidean_distance(a, b);
            if (i == 0 || distance < closest) {
                closest = distance;
            }
        }
        return closest;
	}
    // method for calculating euclidean distance between two <vector>float
    static float vectors_euclidean_distance(const std::vector<float>& a, const std::vector<float>& b) {
        std::vector<float> auxiliary;
        std::transform (a.begin(), a.end(), b.begin(), std::back_inserter(auxiliary),//
                        [](float element1, float element2) {return pow((element1-element2),2);});
        auxiliary.shrink_to_fit();
        return sqrt(std::accumulate(auxiliary.begin(), auxiliary.end(), 0.0));
    }
	bool different(const FaceTrackerData& target, vector< pair<ofVec2f, FaceTrackerData> >& data) {
		vector<FaceTrackerData*> all;
		for(int i = 0; i < data.size(); i++) {
			all.push_back(&data[i].second);
		}
		return different(target, all);
	}
	bool different(const FaceTrackerData& target, const vector<FaceTrackerData*>& data) {
		if(data.empty()) {
			return true;
		}
		return
        closestPosition(target, data) * scalePosition > 2 ||
        closestOrientation(target, data) * scaleOrientation > 2 ||
        closestExpression(target, data) * scaleExpression > 1;
	}
	float distance(const FaceTrackerData& a, const FaceTrackerData& b) {
		float distancePosition = a.position.distance(b.position);
        float distanceScale = abs(a.scale - b.scale);
        float distanceOrientation = a.orientation.distance(b.orientation); // approximation
        float distanceExpression = vectors_euclidean_distance(a.expressions, b.expressions);
        distancePosition *= scalePosition;
        distanceOrientation *= scaleOrientation;
        distanceScale *= scaleScale;
        distanceExpression *= scaleExpression;
        return sqrt(distancePosition * distancePosition +
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
    vector<FaceTrackerData*> nearestX(int X, FaceTrackerData& target, vector<FaceTrackerData*>& data) {
        float lastClosest = 0;
        vector<FaceTrackerData*> closestDataArray;
        for(int x = 0; x < X; x++) {
            float closest = 100000;
            FaceTrackerData* closestData = NULL;
            for(int i = 0; i < data.size(); i++) {
                float distance = FaceCompare::distance(target, *data[i]);
                if((distance < closest) && (distance > lastClosest)) {
                     closest = distance;
                     closestData = data[i];
                }
            }
            lastClosest = closest;
            closestDataArray.push_back(closestData);
        }
        return closestDataArray;
    }
};
