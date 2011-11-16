#pragma once

#include "ofMain.h"
#include "ofxIPhoneTracker.h"
#include "ofxSimpleGuiToo.h"


class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
		vector<ofPoint> trackedPoints;
		vector<ofPoint> trackedVectors;
		int numTrackedPoints;
		ofEasyCam easyCam;
		int radius;
		ofImage earth;
		GLUquadricObj *quadric;
	
	
		ofxSimpleGuiToo gui;

};
