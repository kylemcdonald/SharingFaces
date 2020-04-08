#include "ofMain.h"

class ARKitFaceTracker {
public:
    ofVec2f position;
    float scale;
    ofVec3f orientation;
    vector<float> expressions;
    
    void setup() {
        // instantiate initial values for variables, if necessary
    }
    
    void setPosition(const ofVec2f& newPosition) {
        position = newPosition;
    }
    
    void setOrientation(const ofVec3f& newOrientation) {
        orientation = newOrientation;
    }
    
    void setScale(const float& newScale) {
        scale = newScale;
    }
    
    void setExpressions(const vector<float>& newExpressions) {
        expressions = newExpressions;
    }
    
    
    virtual ofVec2f getPosition() const {
        return this->position;
    };
    
    virtual ofVec3f getOrientation() const {
        return this->orientation;
    };
    
    virtual float getScale() const {
        return this->scale;
    };
    
    virtual vector<float> getExpressions() const {
        return this->expressions;
    };
};
