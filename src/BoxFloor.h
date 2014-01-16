//
//  BoxWall.h
//  StereoPlanes
//
//  Created by Bernardo Schorr on 1/16/14.
//
//

#pragma once

#include "ofMain.h"

class BoxFloor {
    
public:
    void setup(ofParameterGroup * params);
    void draw( ofVec2f dancerPos );
    void update();
    
    //ofParameterGroup * parameters;
    
    ofParameter<float> waveHeight;
    ofParameter<float> speed;
    ofParameter<float> plateauHeight;
    ofParameter<float> lightHeight;
    
    ofLight light;

    
};