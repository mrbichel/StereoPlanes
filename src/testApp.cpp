#include "testApp.h"
#include <OpenGL/OpenGL.h>
#include <ofGLUtils.h>

// Syphon together with 3D primitive and NoFill does not run

vector<ofVec3f> points;

//--------------------------------------------------------------
void testApp::setup()
{
    
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofSetBackgroundAuto(true);
    ofBackground(0);
    
    leftOutputServer.setName("Left");
    rightOutputServer.setName("Right");
    sbsOutputServer.setName("Side By Side");
    
    fbo.allocate(ofGetWidth(), ofGetHeight());
    
    settings.load("stereoplanes.xml");
    
    floor = new StereoPlane("floor");
    floor->setup(1024, 1024, &settings);
    planes.push_back(floor);
    
    wall = new StereoPlane("wall");
    wall->setup(1024, 1024, &settings);
    planes.push_back(wall);
    
    activePlaneIndex = 0;
    activePlane = planes[activePlaneIndex];
    
    //light.setPointLight();
    light.setPosition(2, 2, -2);
    light.setSpotlight();
    light.lookAt(ofVec3f(0,0,0));
    light.setDiffuseColor(ofColor(245,245,170));
    light.setAmbientColor(ofColor(64,84,89));
    light.setSpecularColor(ofColor::white);
    
    dirLight.setSpotlight();
    dirLight.setPosition(-1, -1, 1);
    dirLight.lookAt(ofVec3f(0,0,0));
    dirLight.setDiffuseColor(ofColor(191,191,170));
    
    parameters.setName("Stereo");
    parameters.add(camPos.set("Cam position", ofVec3f(0.,0.,-1), ofVec3f(-2,-2,-8.), ofVec3f(2,2,-0.5)));
    parameters.add(eyeSeperation.set("Eye Seperation", 6.5, 0., 7.));
    
    gui.setup(parameters);
    
}

//--------------------------------------------------------------
void testApp::update()
{
    
    for(int i=0; i<planes.size(); i++) {
        planes[i]->cam.setPosition(camPos.get());
        planes[i]->cam.setPhysicalEyeSeparation(eyeSeperation.get());
        planes[i]->update();
        //cout<<camPos.get()<<endl;
    }
    
}


void testApp::drawFloor() {
    
    glPushMatrix();
    
    ofEnableLighting();
    light.enable();
    dirLight.enable();
    
//    ofBackground(0,0,0,255);
    
    ofSetColor(255,255,255,100);
    //ofDrawGrid(1);
    
    ofPushMatrix();
    //ofRotateX(ofGetElapsedTimef()*100);
    
    ofFill();
    ofSetColor(255,255,255,255);
    //ofDrawBox(0.1);
    //ofDrawBox(0.2);
    ofSetLineWidth(6);
    ofSetBoxResolution(10);
    ofRotateX(ofGetElapsedTimef()*10);
    ofRotateY(ofGetElapsedTimef()*33);
    ofDrawBox(0.5);
    //ofDrawBox(1);
    //ofDrawBox(1.5);
    ofPopMatrix();
   
    light.disable();
    dirLight.disable();
    
    ofDisableLighting();
    
    glPopMatrix();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofEnableDepthTest();
    ofBackground(ofColor(0,0,0));
    
    fbo.begin();
    
    ofClear(0, 0, 0);
    
    floor->beginLeft();
        drawFloor();
    floor->endLeft();
    
    floor->beginRight();
        drawFloor();
    floor->endRight();
    
    wall->beginLeft();
        drawFloor();
    wall->endLeft();
    
    wall->beginRight();
        drawFloor();
    wall->endRight();
    
    ofDisableDepthTest();
    
    ofSetColor(255);
    ofSetLineWidth(2);
    ofFill();
    
    if(showGrid) {
        for(int i=0; i<planes.size(); i++) {
            planes[i]->drawGrids();
        }
    }
    
    for(int i=0; i<planes.size(); i++) {
        planes[i]->draw();
    }
    
    fbo.end();
    
    sbsOutputServer.publishTexture(&fbo.getTextureReference());
    
    ofPushMatrix();
    fbo.draw(0, 0);
    ofPopMatrix();
    
    ofSetColor(255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 40, 40);
    activePlane->drawInfo();
    
    gui.draw();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

	if (key == 'f')
	{
		ofToggleFullscreen();
	} else if (key == 'g')
	{
		showGrid = !showGrid;
	} else if (key == 'd')
	{
		showDots =! showDots;
	} else if(key == 'r') {
        activePlane->activateRightControl();
    } else if(key == 'l') {
        activePlane->activateLeftControl();
    } else if(key == 'p') {
        
        if(activePlaneIndex == planes.size()-1) {
            activePlaneIndex = 0;
        } else {
            activePlaneIndex++;
        }
        
        activePlane->deactivateControl();
        activePlane = planes[activePlaneIndex];
    }
    
}


//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{
    //ofVec3f mousePoint(x - ofGetWidth()/2,y-ofGetHeight()/2, 400);
    //points.push_back(mousePoint);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{
    
}


void testApp::exit() {
    
    for(int i=0; i < planes.size(); i++) {
        planes[i]->exit();
    }
    
    settings.save("stereoplanes.xml");
    cout<<"exit"<<endl;
}