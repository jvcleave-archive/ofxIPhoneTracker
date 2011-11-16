#pragma once

#include "ofMain.h"
#include "ofxIPhoneTracker.h"
#include "ofxSimpleGuiToo.h"


class testApp : public ofBaseApp{

	public:
	void setup();
	void start();
	void update();
	void draw();
	void createSphere     
    ( 
		 ofTexture* texture, 
		 ofVec3f center, 
		 double radius,
		 int n, 
		 double theta1   = 0, 
		 double theta2   = TWO_PI, 
		 double phi1     = -PI * 0.5,
		 double phi2     = PI * 0.5
	 );
	
	void drawSphere ( ofTexture* texture );
    void drawLocations ( vector<ofVec2f>& locations, float radius );
    
    ofVec3f sphericalToCartesian ( float lat, float lon, float radius );
	
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofxIPhoneTracker *tracker;



	ofxSimpleGuiToo gui;


	bool bMouseDown;
    ofVec2f mouseDown;
    ofVec2f mouseLast;
    ofVec2f mouseVel;
    ofVec2f mouseVelEase;
    ofQuaternion quat;
    ofMatrix4x4 mat;
    
    vector<ofVec2f> locations;

	ofImage earthImage;
	ofTexture* earthTexture;
	ofVec3f earthCenter;
	float earthRadius;
	float* earth_ver;
	float* earth_tex;
	float* earth_nor;
	int earth_rows;
	int earth_row_length;

	void drawGlobe();
	void updateMouse();
};
