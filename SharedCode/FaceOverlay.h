#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxTiming.h"

class FaceOverlay {
private:
    static void drawCurve(const ofPolyline& polyline) {
        if(polyline.size() == 0) {
            return;
        }
        ofBeginShape();
        const auto& vertices = polyline.getVertices();
        ofCurveVertex(vertices.front().x, vertices.front().y);
        for(int i = 0; i < vertices.size(); i++) {
            ofCurveVertex(vertices[i].x, vertices[i].y);
        }
        ofCurveVertex(vertices.back().x, vertices.back().y);
        ofEndShape();
    }
    
    static ofPolyline buildCircle(const ofVec2f& center, float radius, int resolution = 32) {
        ofPolyline polyline;
        ofVec2f base(radius, 0);
        for(int i = 0; i < resolution; i++) {
            float angle = 360 * (float) i / resolution;
            ofVec2f cur = center + base.getRotated(angle);
            polyline.addVertex((ofVec3f) cur);
        }
        polyline.close();
        return polyline;
    }
    
    static float determinant(const ofVec2f& start, const ofVec2f& end, const ofVec2f& point) {
        return (end.x - start.x) * (point.y - start.y) - (end.y - start.y) * (point.x - start.x);
    }
    
    static void divide(const ofVec2f& a, const ofVec2f& b,
                const ofVec2f& left, const ofVec2f& right,
                ofMesh& top, ofMesh& bottom) {
        ofVec2f avg = a.getInterpolated(b, 0.5);
        if(determinant(left, right, avg) < 0) {
            top.addVertex((ofVec3f) a);
            top.addVertex((ofVec3f) b);
        } else {
            bottom.addVertex((ofVec3f) a);
            bottom.addVertex((ofVec3f) b);
        }
    }
    
    static void divide(const ofPolyline& poly,
                const ofVec2f& left, const ofVec2f& right,
                ofMesh& top, ofMesh& bottom) {
        top.setMode(OF_PRIMITIVE_LINES);
        bottom.setMode(OF_PRIMITIVE_LINES);
        const auto& vertices = poly.getVertices();
        for(int i = 0; i + 1 < vertices.size(); i++) {
            divide(vertices[i], vertices[i + 1], left, right, top, bottom);
        }
        if(poly.isClosed()) {
            divide(vertices.back(), vertices.front(), left, right, top, bottom);
        }
    }
    
public:
    Hysteresis noseHysteresis, mouthHysteresis;
    float noseAngle = 15;
    float nostrilWidth = .06;
    float lipWidth = .7;
    
    FaceOverlay() {
        noseHysteresis.setDelay(1000);
        mouthHysteresis.setDelay(1000);
    }
    void draw(ofxFaceTracker& tracker) {
        if(noseHysteresis.set(abs(tracker.getOrientation().y) > ofDegToRad(noseAngle))) {
            tracker.getImageFeature(ofxFaceTracker::NOSE_BRIDGE).draw();
        }
        
        ofPolyline eyeLeft = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
        ofPolyline eyeRight = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
        ofVec2f eyeLeftLeft = tracker.getImagePoint(36);
        ofVec2f eyeLeftRight = tracker.getImagePoint(39);
        ofVec2f eyeRightLeft = tracker.getImagePoint(42);
        ofVec2f eyeRightRight = tracker.getImagePoint(45);
        float eyeRadius = tracker.getScale() * 3;
        
        float lidHeight = .5;
        ofVec2f eyeLeftLidLeft = glm::lerp(tracker.getImagePoint(36), tracker.getImagePoint(37), lidHeight);
        ofVec2f eyeLeftLidRight = glm::lerp(tracker.getImagePoint(38), tracker.getImagePoint(39), lidHeight);
        ofVec2f eyeRightLidLeft = glm::lerp(tracker.getImagePoint(42), tracker.getImagePoint(43), lidHeight);
        ofVec2f eyeRightLidRight = glm::lerp(tracker.getImagePoint(44), tracker.getImagePoint(45), lidHeight);
        
        ofVec2f eyeCenterLeft = eyeLeftLidLeft.getInterpolated(eyeLeftLidRight, .5);
        ofVec2f eyeCenterRight = eyeRightLidLeft.getInterpolated(eyeRightLidRight, .5);
        
        float irisSize = .5;
        ofMesh leftTop, leftBottom;
        ofPolyline leftCircle = buildCircle(eyeCenterLeft, eyeRadius * irisSize);
        divide(leftCircle, eyeLeftLidLeft, eyeLeftLidRight, leftTop, leftBottom);
        ofMesh rightTop, rightBottom;
        ofPolyline rightCircle = buildCircle(eyeCenterRight, eyeRadius * irisSize);
        divide(rightCircle, eyeRightLidLeft, eyeRightLidRight, rightTop, rightBottom);
        
        leftBottom.draw();
        rightBottom.draw();
        
        ofPolyline lowerLip;
        lowerLip.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(59), tracker.getImagePoint(48), lipWidth));
        lowerLip.addVertex((ofVec3f) tracker.getImagePoint(59));
        lowerLip.addVertex((ofVec3f) tracker.getImagePoint(58));
        lowerLip.addVertex((ofVec3f) tracker.getImagePoint(57));
        lowerLip.addVertex((ofVec3f) tracker.getImagePoint(56));
        lowerLip.addVertex((ofVec3f) tracker.getImagePoint(55));
        lowerLip.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(55), tracker.getImagePoint(54), lipWidth));
        drawCurve(lowerLip);
        
        ofPolyline innerLip;
        innerLip.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(60), tracker.getImagePoint(48), lipWidth));
        innerLip.addVertex((ofVec3f) tracker.getImagePoint(60));
        innerLip.addVertex((ofVec3f) tracker.getImagePoint(61));
        innerLip.addVertex((ofVec3f) tracker.getImagePoint(62));
        innerLip.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(62), tracker.getImagePoint(54), lipWidth));
        drawCurve(innerLip);
        
        // if mouth is open than some amount, draw this line
        if(mouthHysteresis.set(tracker.getGesture(ofxFaceTracker::MOUTH_HEIGHT) > 2)) {
            ofPolyline innerLowerLip;
            innerLowerLip.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(65), tracker.getImagePoint(48), lipWidth));
            innerLowerLip.addVertex((ofVec3f) tracker.getImagePoint(65));
            innerLowerLip.addVertex((ofVec3f) tracker.getImagePoint(64));
            innerLowerLip.addVertex((ofVec3f) tracker.getImagePoint(63));
            innerLowerLip.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(63), tracker.getImagePoint(54), lipWidth));
            drawCurve(innerLowerLip);
        }
        
        // nose
        ofPolyline nose;
        nose.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(31), tracker.getImagePoint(4), nostrilWidth));
        nose.addVertex((ofVec3f) tracker.getImagePoint(31));
        nose.addVertex((ofVec3f) tracker.getImagePoint(32));
        nose.addVertex((ofVec3f) tracker.getImagePoint(33));
        nose.addVertex((ofVec3f) tracker.getImagePoint(34));
        nose.addVertex((ofVec3f) tracker.getImagePoint(35));
        nose.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(35), tracker.getImagePoint(12), nostrilWidth));
        drawCurve(nose);
        
        ofPolyline upperLip;
        upperLip.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(49), tracker.getImagePoint(48), lipWidth));
        upperLip.addVertex((ofVec3f) tracker.getImagePoint(49));
        upperLip.addVertex((ofVec3f) tracker.getImagePoint(50));
        upperLip.addVertex((ofVec3f) tracker.getImagePoint(51));
        upperLip.addVertex((ofVec3f) tracker.getImagePoint(52));
        upperLip.addVertex((ofVec3f) tracker.getImagePoint(53));
        upperLip.addVertex((ofVec3f) glm::lerp(tracker.getImagePoint(53), tracker.getImagePoint(54), lipWidth));
        drawCurve(upperLip);
        
        drawCurve(tracker.getImageFeature(ofxFaceTracker::LEFT_EYE_TOP));
        drawCurve(tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE_TOP));
        drawCurve(tracker.getImageFeature(ofxFaceTracker::LEFT_EYEBROW));
        drawCurve(tracker.getImageFeature(ofxFaceTracker::RIGHT_EYEBROW));
        drawCurve(tracker.getImageFeature(ofxFaceTracker::JAW));
    }
};
