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
    parameters.add(dancerEllipseSize.set("Dancer Ellipse Size", 0., 0., .5));
    parameters.add(dancerEllipseBrightness.set("Dancer Ellipse Brightness", 0., 0., 1.));
    parameters.add(dancerPos.set("Dancer position", ofVec2f(-1.,-1.), ofVec2f(-1,-1), ofVec2f(1,1)));
    
    //    sync.setup(parameters, 9002, "localhost", 8000);
    
    gui.setup(parameters);
    
    oscReceiver.setup(9001);
    
    world.setup();
	world.setGravity(ofVec3f(0.f, 0.f, 9.8f));
    
    ground.create( world.world, ofVec3f(0., 0, 0.5), 0., 100.f, 100.f, 1.f );
	ground.setProperties(.25, .95);
	ground.add();
    
    
    
    //  TODO: Operator grabbing of bullet objects from first view?
    //	world.enableGrabbing();
    //	world.enableDebugDraw();
    //	world.setCamera(&camera);
    
    // Voronoi wall
    vbounds.set(-2, -2, 4, 4);
    voronoi.setBounds(vbounds);
    
    int n = 100;
    for(int i=0; i<n; i++) {
        vpts.push_back(ofRandomPointInRect(vbounds));
    }
    
    vpts.push_back(ofVec3f(0,0,0));

    
    voronoi.clear();
    for(int i=0; i<vpts.size(); i++) {
        voronoi.addPoint(vpts[i]);
    }
    voronoi.generateVoronoi();
    
} 

//--------------------------------------------------------------
void testApp::update()
{
    while(oscReceiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		oscReceiver.getNextMessage(&m);
        
		if(m.getAddress() == "/Camera/x"){
            ofVec3f pos = camPos.get();
            pos.x = m.getArgAsFloat(0);
			camPos.set(pos);
            
		} else if(m.getAddress() == "/Camera/y"){
            ofVec3f pos = camPos.get();
            pos.y = m.getArgAsFloat(0);
			camPos.set(pos);
            
		} else if(m.getAddress() == "/Cameraz/x"){
            
            ofVec3f pos = camPos.get();
            pos.z = m.getArgAsFloat(0);
			camPos.set(pos);
            
		} else if(m.getAddress() == "/eyeSeperation/x"){
			eyeSeperation.set(m.getArgAsFloat(0));
            
		}
    }

    //TODO: Camera frustrums share position, but with individual viewports
    for(int i=0; i<planes.size(); i++) {
        planes[i]->cam.setPosition(camPos.get());
        planes[i]->cam.setPhysicalEyeSeparation(eyeSeperation.get());
        planes[i]->update();
        //cout<<camPos.get()<<endl;
    }
    
    if(addSphere){
        ofxBulletSphere * sphere = new ofxBulletSphere();
        sphere->create(world.world, ofVec3f(ofRandom(-0.1,0.1), ofRandom(-0.1,0.1), -1), 0.05, ofRandom(0.02,0.05));
        spheres.push_back(sphere);
        sphere->add();
        addSphere = false;
    }
    
    world.update();
    
}


void testApp::drawVoronoiWall() {
    
    //TODO:Factor out to seperate class
    
    glPushMatrix();
    
    ofNoFill();
    
    light.enable();
    dirLight.enable();
    
    //voronoi.draw();
    voronoi.getPoints().size();
    
    
    
    for(int i=0; i < voronoi.cells.size(); i++) {
        
        ofMesh vcell;
        
        vcell.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        
        for(int v=0; v<voronoi.cells[i].pts.size(); v++) {
            
            vcell.addVertex(voronoi.cells[i].pts[v]);
            
            ofColor col;
            if(i%2 == 0) {
                col.set(255,255,50);
            } else {
                col.set(50,255,255);
            
            
            vcell.addColor(col);
        }
        ofPushMatrix();
        
        ofTranslate(0, 0, ofSignedNoise(ofGetElapsedTimef()/8 + i*2)*0.6);
            ofScale(0.9, 0.9, 1);
            //ofRotateX(ofSignedNoise(ofGetElapsedTimef()/8 +i)*80);
            ofRotateY(ofSignedNoise(ofGetElapsedTimef()/8 +i)*20);
        //vcell.draw();
            ofSetLineWidth(10);
        //vcell.setMode(OF_PRIMITIVE_LINE_STRI);
            vcell.draw();
        
            ofPushMatrix();
                ofTranslate(0, 0, 0.05);
                //
                //vcell.draw();
            
            
            ofPopMatrix();
        //vcell.drawWireframe();
        
        
        ofPopMatrix();
        }
    }
    
    light.disable();
    dirLight.disable();
    
    ofDisableLighting();
    glPopMatrix();
    
}

void testApp::drawBulletFloor(){
    
    //TODO:Factor out to seperate class

    ofPushMatrix(); {
        //ofRotateY(90);
        //world.drawDebug();
        
        for(int i=0; i<spheres.size(); i++) {
            spheres[i]->draw();
        }
        
        
    } ofPopMatrix();
    
}

void testApp::drawFloor() {
    
    glPushMatrix();{
        
        ofEnableLighting();
        light.enable();
        dirLight.enable();
        
        //TODO: Find better fog parameters...
        /*glEnable (GL_FOG);
         glFogi (GL_FOG_MODE, GL_EXP2);
         glHint (GL_FOG_HINT, GL_NICEST);
         glFogi(GL_FOG_DENSITY, 1);
         */
        
        /** A rotating box intersects the floor
        ofPushMatrix();
        ofFill();
        ofSetColor(255,255,255,255);
        ofSetBoxResolution(10);
        ofRotateX(ofGetElapsedTimef()*10);
        ofDrawBox(0.5);
        ofPopMatrix();
        **/
        
        // A WHITE FLOOR
        ofSetColor(255,255,255,255);
        ofRect(-1,-1, 2, 2);
        
        drawBulletFloor();
        
        /** Some random moving balls
         for (int i = 0; i< 200; i++) {
            ofDrawSphere(ofSignedNoise((ofGetElapsedTimef()*0.01)+i, 0, 0), ofSignedNoise((ofGetElapsedTimef()*0.01)+i, (ofGetElapsedTimef()*0.01)+i, 0), ofSignedNoise(0,0,(ofGetElapsedTimef()*0.01)+i)*0.5,  0.05);
         }
         **/
        
        
        light.disable();
        dirLight.disable();
        ofDisableLighting();
        
        ofDisableDepthTest();
        ofSetColor(0,0,0,255);
        ofEllipse(dancerPos->x, dancerPos->y, 0.02, 0.02);
        ofEnableDepthTest();
        
        //glDisable(GL_FOG);
        
    } glPopMatrix();
    
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
    drawVoronoiWall();
    wall->endLeft();
    
    wall->beginRight();
    drawVoronoiWall();
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
	} else if(key == 's') {
        addSphere = true;
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