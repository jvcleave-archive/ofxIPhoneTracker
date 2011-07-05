#include "testApp.h"


int currentPointIndex = 0;
bool doUseSampleFile = false;
//--------------------------------------------------------------
void testApp::setup()
{
	
	radius = 300;
	tracker = new ofxIPhoneTracker();
	
	//doWifi = true finds a lot more data but far less accurate. will likely beachball for a few moments
	tracker->doWifi = false;
	
	if(doUseSampleFile)
	{
		string sampleFilePath = ofToDataPath("Jason Van Cleave’s iPhone", true);
		tracker->tryToLoadLocationDB(sampleFilePath.c_str(), "Jason Van Cleave’s iPhone");
		cout << "USING SAMPLE FILE" << sampleFilePath << endl;
	}else 
	{
		cout << "WILL SEARCH SYSTEM FOR FILE" << endl;
		tracker->doCopyFilesToDataPath = true;
		tracker->loadLocationDB();
	}
	for (int i=0; i<tracker->bucketObjects.size(); i++)
	{
		float latitude = tracker->bucketObjects[i].latitude;
		float longitude = tracker->bucketObjects[i].longitude;
		float x = radius * cos(latitude) * cos(longitude);
		float y = radius * cos(latitude) * sin(longitude);
		float z = radius * sin(latitude);	
		trackedPoints.push_back(ofPoint(x, y, z));
	}
	numTrackedPoints = trackedPoints.size();
}

//--------------------------------------------------------------
void testApp::update()
{
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	ofPushMatrix();
		easyCam.begin();
		ofSetColor(0, 0, 255);
		ofSphere(radius);
			if (currentPointIndex<numTrackedPoints-1 && (ofGetFrameNum() % 1 == 0))
			{
				currentPointIndex++;
				//cout << "currentPointIndex: " << currentPointIndex << endl;
			}
			ofPushStyle();
				
				for (int i=0; i<numTrackedPoints; i++)
				{
					
					if (i<numTrackedPoints-2 && i<currentPointIndex)
					{
						ofSetColor(0, 255, 255);
						ofCircle(trackedPoints[i], 1);
						ofSetColor(0);
						ofLine(trackedPoints[i].x, trackedPoints[i].y, trackedPoints[i].z, trackedPoints[i+1].x, trackedPoints[i+1].y, trackedPoints[i+1].z);
					}
					
				}
			ofPopStyle();
		//easyCam.orbit(trackedPoints[currentPointIndex].x, trackedPoints[currentPointIndex].y, radius, ofVec3f(trackedPoints[currentPointIndex]));
		easyCam.end();
	ofPopMatrix();
	ofDrawBitmapString("TRACKED POINTS:" +ofToString(trackedPoints.size()), 20, 20);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}