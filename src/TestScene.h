//
//  TestScene.h
//  StereoPlanes
//
//  Created by Johan Bichel Lindegaard on 21/01/14.
//
//

#pragma once
#include "ContentScene.h"
#include "ofMain.h"

class TestScene : public ContentScene {
    
public:
    void draw(int _surfaceId);
    void update();
    void setup();
    
    ofLight light;
    
};
