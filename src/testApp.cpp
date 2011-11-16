#include "testApp.h"


int currentPointIndex = 0;
bool doUseSampleFile = true;
bool doAnimate = false;
//--------------------------------------------------------------
void testApp::setup()
{
	gui.addButton("whatever", doAnimate);
	gui.show();
	radius = 200;
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
	for (int i=0; i<tracker->bucketObjects.size(); i++)
	{
		ofPoint result;
		float lat = tracker->bucketObjects[i].latitude;
		float lon = tracker->bucketObjects[i].longitude;
		//lat	*= -1;		// inverse latitude.
		//lat += 90;		// latitude offset to match geometry of the sphere.
		//lon *= -1;		// inverse longitude.
		//lon -= 90;		// longitude offset to match geometry of the sphere.
		
		lat *= DEG_TO_RAD;
		lon *= DEG_TO_RAD;
		
		result.x = radius * sin( lat ) * cos( lon );
		result.y = radius * sin( lat ) * sin( lon );
		result.z = radius * cos( lat );
		trackedVectors.push_back(result);
	}
	numTrackedPoints = trackedPoints.size();
	glEnable(GL_DEPTH_TEST); //enable depth comparisons and update the depth buffer
	ofDisableArbTex(); //needed for textures to work with gluSphere
	earth.loadImage("earth.jpg");
	//earth.loadImage("EarthMap_1024x512_hFlip.jpg");
	earth.mirror(true, false);
	//
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricNormals(quadric, GLU_SMOOTH);
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
		//ofSetColor(0, 0, 255);
		earth.getTextureReference().bind();
	//	ofSphere
		gluSphere(quadric, radius, 100, 100);
		earth.getTextureReference().unbind();
	ofPushStyle();
	if (doAnimate) 
	{
		if (currentPointIndex<numTrackedPoints-1 && (ofGetFrameNum() % 1 == 0))
		{
			currentPointIndex++;
			//cout << "currentPointIndex: " << currentPointIndex << endl;
		}
		
		
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
		
	}else {
		for (int i=0; i<trackedVectors.size(); i++)
		{
			
			if (i<trackedVectors.size()-1)
			{
				ofSetColor(ofRandom(255));
				ofCircle(trackedVectors[i], 1);
				ofSetColor(0);
				//ofLine(trackedPoints[i].x, trackedPoints[i].y, trackedPoints[i].z, trackedPoints[i+1].x, trackedPoints[i+1].y, trackedPoints[i+1].z);
			}
			
		}
	}
	ofPopStyle();

			
	
		//easyCam.orbit(trackedPoints[currentPointIndex].x, trackedPoints[currentPointIndex].y, radius, ofVec3f(trackedPoints[currentPointIndex]));
		easyCam.end();
	ofPopMatrix();
	ofDrawBitmapString("TRACKED POINTS:" +ofToString(trackedPoints.size()), 20, 20);
	
	gui.draw();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == ' ') {
		
		string fileName = "iPhoneData_";
		fileName += ofToString(ofGetTimestampString());
		fileName += "_ascii.json";
		ofFile file;
		if(file.open(fileName, ofFile::WriteOnly))
		{
			
			file << "\"1990\" [" << ends;
			for (int i=0; i<tracker->bucketObjects.size(); i++)
			{
				
				file << ofToString(tracker->bucketObjects[i].latitude)+"," << ends;
				file << ofToString(tracker->bucketObjects[i].longitude)+"," << ends;
				file << "1" << ends;
				if (i<(tracker->bucketObjects.size()-1))
				{
					file << "," << ends;
				}
			}
			file << "]" << endl;
		}
	}
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