#pragma once

#include "ofxCv.h"
#include "ofxOpenCv.h"

class BlurDetection {
public:
    ofVec2f position;
    float scale;
    ofVec3f orientation;
    
    void setup() {
        // instantiate initial values for variables, if necessary
    }
    
    float measureBlur(ofImage& img) {
        cv::Mat src = ofxCv::toCv(img);
        cv::cvtColor(src, src, cv::COLOR_RGB2GRAY);
        
        cv::Mat dst;
        cv::Laplacian(src, dst, CV_32F);
        
        cv::Scalar mean, stddev;
        cv::meanStdDev(dst, mean, stddev);
        float blurMeasure = stddev.val[0]*stddev.val[0];
        
        return blurMeasure;
    }
};
