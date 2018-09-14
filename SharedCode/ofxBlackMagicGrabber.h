#pragma once

#include "ofMain.h"
#include "ofxBlackMagic.h"
#include "ofxCv.h"

class ofxBlackMagicGrabber : public ofVideoGrabber {
private:
    int framerate = 0;
    int deviceID = 0;
    bool m_isFrameNew = false;
    bool m_isInitialized = false;
    ofPixels colorPixels;
    ofTexture* colorTexture = nullptr;
public:
    ofxBlackMagic camera;
    
    ~ofxBlackMagicGrabber() { };
    
    void setDesiredFrameRate(int framerate) override {
        this->framerate = framerate;
    }
    void setDeviceID(int deviceID) override {
        this->deviceID = deviceID;
    }
    bool setPixelFormat(ofPixelFormat pixelFormat) override {}
    ofPixelFormat getPixelFormat() const override {
        return OF_PIXELS_RGB;
    }
    std::vector<ofVideoDevice> listDevices() const override {
        std::vector<ofVideoDevice> devices;
        return devices; // placeholder
    }
    bool setup(int width, int height) override {
        return camera.setup(width, height, framerate, deviceID);
    }
    void close() override {
        camera.close();
    }
    void update() override {
        m_isFrameNew = camera.update();
        if(m_isFrameNew) {
            colorTexture = &camera.getColorTexture();
            
            // because @smallfly switched color conversion to RGBA
            // https://github.com/kylemcdonald/ofxBlackmagic/pull/8
            // we have to do a convert instead of a copy
            // this would all be faster if we converted directly from
            // native yuy2 to rgb and gray, and used gray for facetracking
            // and rgb for saving (or even saved yuy2 directly, 2MB)
            ofxCv::convertColor(camera.getColorPixels(), colorPixels, CV_RGBA2RGB);
            
            m_isInitialized = true;
        }
    }
    bool isFrameNew() const override {
        return m_isFrameNew;
    }
    bool isInitialized() const override {
        return m_isInitialized;
    }
    ofPixels& getPixels() override {
        return colorPixels;
    }
    const ofPixels& getPixels() const override {
        return colorPixels;
    }
    float getWidth() const override {
        return camera.getWidth();
    }
    float getHeight() const override {
        return camera.getHeight();
    }
    void draw(float x, float y, float w, float h) const override {
        if(colorTexture != nullptr) {
            colorTexture->draw(x, y, w, h);
        }
    }
    void setVerbose(bool verbose) override {};
    void videoSettings() override {};
};
