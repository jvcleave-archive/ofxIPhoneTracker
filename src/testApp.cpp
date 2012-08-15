#include "testApp.h"


int currentPointIndex = 0;
bool doUseSampleFile = true;
bool doAnimate = false;
bool doWifi = false;
bool doStart = false;
bool hasStarted = false;
//--------------------------------------------------------------
void testApp::setup()
{
	ofDisableArbTex();
	gui.addToggle("doUseSampleFile", doUseSampleFile);
	gui.addToggle("doWifi", doWifi);
	gui.addToggle("doStart", doStart);
	gui.show();
	
	earthImage.loadImage( "earth_4096x2048.png" );
	
    earthTexture = &earthImage.getTextureReference();
	
    earthCenter.x = ofGetWidth()  * 0.5;
    earthCenter.y = ofGetHeight() * 0.5;
    earthCenter.z = 0;
    
    earthRadius = MIN( ofGetWidth(), ofGetHeight() ) * 0.3;
    
    earth_ver = NULL;
    earth_tex = NULL;
    earth_nor = NULL;
    
    createSphere( earthTexture, ofVec3f(), earthRadius, 200 );
	camera.setTarget(ofVec3f(earthCenter.x, earthCenter.y, earthCenter.z));
	ofEnableAlphaBlending();

}
void testApp::start()
{
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
		locations.push_back( ofVec2f(latitude, longitude));
	}

		
	hasStarted = true;
}
//--------------------------------------------------------------
void testApp::update()
{
	if (doStart) 
	{
		doStart = false;
		
		start();
		gui.hide();
		
	}
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	//updateMouse();
}
//--------------------------------------------------------------
void testApp :: updateMouse()
{
    if( bMouseDown )
    {
        mouseVel  = mouseDown - mouseLast;
        mouseLast = mouseDown;
    }
    else
    {
        mouseVel *= 0.92;
    }
    
    mouseVelEase += ( mouseVel - mouseVelEase ) * 0.2;
    
    if( mouseVelEase.length() < 0.000001 )
        return;
    
    ofVec2f mousePerp;
    mousePerp = mouseVelEase.getPerpendicular();
    
    float angle;
    angle = mouseVelEase.length() * 0.1;
    
    ofQuaternion q;
    q.makeRotate( angle, ofVec3f( mousePerp ) );
    
    quat *= q;
    
    mat.makeRotationMatrix( quat );
}

/*
 Code modified from original example :: http://paulbourke.net/texture_colour/texturemap/
 
 Create a sphere centered at c, with radius r, and precision n
 Draw a point for zero radius spheres
 Use CCW facet ordering
 "method" is 0 for quads, 1 for triangles
 (quads look nicer in wireframe mode)
 Partial spheres can be created using theta1->theta2, phi1->phi2
 in radians 0 < theta < 2pi, -pi/2 < phi < pi/2
 */
void testApp :: createSphere ( ofTexture* texture, ofVec3f center, double radius, int n, double theta1, double theta2, double phi1, double phi2 )
{
    if( ( radius <= 0 ) || ( n < 4 ) )
        return;
	
    if( earth_ver ) delete[] earth_ver;
    if( earth_tex ) delete[] earth_tex;
    if( earth_nor ) delete[] earth_nor;
    
    int r = n / 2;
    int l = ( n + 1 ) * 2;
    int t = r * l;
    earth_ver = new float[ t * 3 ];
    earth_tex = new float[ t * 2 ];
    earth_nor = new float[ t * 3 ];
    earth_rows = r;
    earth_row_length = l;
    
    int i, j, k;
    double t1, t2, t3;
    ofVec3f e, p;
    ofVec2f tp;
    
    for( j=0; j<r; j++ )
    {
        t1 = phi1 + j * ( phi2 - phi1 ) / ( n / 2 );
        t2 = phi1 + ( j + 1 ) * ( phi2 - phi1 ) / ( n / 2 );
        
        for( i=0; i<=n; i++ ) 
        {
            k = j * l + i * 2;
            
            t3 = theta1 + i * ( theta2 - theta1 ) / n;
            
            earth_nor[ k * 3 + 0 ] = e.x = cos( t1 ) * cos( t3 );
            earth_nor[ k * 3 + 1 ] = e.y = sin( t1 );
            earth_nor[ k * 3 + 2 ] = e.z = cos( t1 ) * sin( t3 );
            
            earth_ver[ k * 3 + 0 ] = p.x = center.x + radius * e.x;
            earth_ver[ k * 3 + 1 ] = p.y = center.y + radius * e.y;
            earth_ver[ k * 3 + 2 ] = p.z = center.z + radius * e.z;
            
            tp.x = 1 - i / (double)n;
            tp.y = 2 * j / (double)n;
            tp = texture->getCoordFromPercent( tp.x, tp.y );
            
            earth_tex[ k * 2 + 0 ] = tp.x;
            earth_tex[ k * 2 + 1 ] = tp.y;
            
            k += 1;
            
            earth_nor[ k * 3 + 0 ] = e.x = cos( t2 ) * cos( t3 );
            earth_nor[ k * 3 + 1 ] = e.y = sin( t2 );
            earth_nor[ k * 3 + 2 ] = e.z = cos( t2 ) * sin( t3 );
            
            earth_ver[ k * 3 + 0 ] = p.x = center.x + radius * e.x;
            earth_ver[ k * 3 + 1 ] = p.y = center.y + radius * e.y;
            earth_ver[ k * 3 + 2 ] = p.z = center.z + radius * e.z;
            
            tp.x = 1 - i / (double)n;
            tp.y = 2 * ( j + 1 ) / (double)n;
            tp = texture->getCoordFromPercent( tp.x, tp.y );
            
            earth_tex[ k * 2 + 0 ] = tp.x;
            earth_tex[ k * 2 + 1 ] = tp.y;
        }
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
	
	if (hasStarted)
	{
		camera.begin();
		
		drawGlobe();
		//camera.setTarget(ofVec3f (earthCenter.x, earthCenter.y, earthCenter.z));
		camera.end();
	}
	
	gui.draw();
	ofDrawBitmapString("NUMBER OF LOCATIONS: " +ofToString(locations.size()), 20, 20);
}
//--------------------------------------------------------------
void testApp::drawGlobe()
{
	glEnable( GL_DEPTH_TEST );
    glEnable( GL_SMOOTH );
    
	glPushMatrix();
	
	glTranslatef( earthCenter.x, earthCenter.y, earthCenter.z );
	
    glMultMatrixf( mat.getPtr() );
    
    drawSphere( earthTexture );
    drawLocations( locations, earthRadius );
	
	glPopMatrix();
	
}

void testApp :: drawSphere ( ofTexture* texture )
{
    texture->bind();
    
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    
    glVertexPointer( 3, GL_FLOAT, 0, earth_ver );
    glTexCoordPointer( 2, GL_FLOAT, 0, earth_tex );
    glNormalPointer( GL_FLOAT, 0, earth_nor );
    
    for( int i=0; i<earth_rows; i++ )
    {
        glDrawArrays( GL_TRIANGLE_STRIP, i * earth_row_length, earth_row_length );
    }
    
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    
    texture->unbind();
}

void testApp :: drawLocations ( vector<ofVec2f>& locations, float radius )
{
	ofPushStyle();
    ofFill();
    ofSetColor( ofRandom(255), ofRandom(255), ofRandom(255) );
    
    for( int i=0; i<locations.size(); i++ )
    {
        ofVec3f loc = sphericalToCartesian( locations[ i ].x, locations[ i ].y, radius * 1.01 );
        ofVec3f dir = loc.getNormalized();
        ofVec3f up  = dir.getPerpendicular( ofVec3f( 0, 1, 0 ) );
        ofVec3f rgh = dir.getCrossed( up );
        
        ofMatrix4x4 mat;
        mat.set(
				rgh.x, rgh.y, rgh.z, 0,
				up.x,  up.y,  up.z, 0,
				dir.x, dir.y, dir.z, 0,
				loc.x, loc.y, loc.z, 1
				);
        
        glPushMatrix();
        glMultMatrixf( mat.getPtr() );
        {
            ofCircle( 0, 0, 2 );
        }
        glPopMatrix();
    }
    ofPopStyle();
}

//---------------------------------------------------------------------------------
ofVec3f testApp :: sphericalToCartesian( float lat, float lon, float radius )
{
	ofVec3f result;
    
	lat	*= -1;		// inverse latitude.
	lat += 90;		// latitude offset to match geometry of the sphere.
	lon *= -1;		// inverse longitude.
	lon += 180;		// longitude offset to match geometry of the sphere.
    
	lat *= DEG_TO_RAD;
	lon *= DEG_TO_RAD;
	
	result.x = radius * sin( lat ) * cos( lon );
	result.y = radius * sin( lat ) * sin( lon );
	result.z = radius * cos( lat );
    result.rotate( 90, ofVec3f( 1, 0, 0 ) );
    
	return result;
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
	//mouseDown = ofVec2f( x, y );
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	/*bMouseDown = true;
    mouseDown = ofVec2f( x, y );
    mouseLast = ofVec2f( x, y );
    mouseVel  = ofVec2f( 0, 0 );*/
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	//bMouseDown = false;

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