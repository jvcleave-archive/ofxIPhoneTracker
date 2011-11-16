/*
 *  ofxGameCamera.h
 *
 *  Created by James George on 1/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"

class ofxGameCamera : public ofCamera {
  public:
	
	ofxGameCamera();
	
	float sensitivityX;
	float sensitivityY;
	
	float minimumX;
	float maximumX;

	float minimumY;
	float maximumY;
	
	float rotationX;
	float rotationY;
	float rotationZ;	
	
	float speed;
	
	virtual void begin(ofRectangle rect = ofGetWindowRect());
	void updateRotation();
	
	ofVec2f lastMouse;
	bool usemouse;

	bool useArrowKeys;
	
	bool autosavePosition;
	
	string cameraPositionFile;
	void saveCameraPosition();
	void loadCameraPosition();
	
	
};
