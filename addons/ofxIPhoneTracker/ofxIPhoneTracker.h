//
//  ofxIPhoneTracker.h
//  ofxIPhoneTracker
//
//  Created by jason van cleave on 4/23/11.
//
// code lifted from Pete Warden's original ofxIPhoneTracker app, therefore this code is GPL
// https://github.com/petewarden/iPhoneTracker
/***********************************************************************************
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************************/

#pragma once

#include "ofMain.h"

struct Location
{
	int count;
	string latitude_string;
	string longitude_string;
	string time_string;
	
};
struct BucketObject
{
	float latitude;
	float longitude;
	float timestamp;
	float unixTimestamp;
	float timeBucket;
	float latitude_index;  
	float longitude_index;
	ofPoint point;
};

//========================================================================
class ofxIPhoneTracker
{

	public:
		ofxIPhoneTracker();
		void loadLocationDB();
		bool tryToLoadLocationDB(const char * locationDBPath, const char * deviceName);
		vector<Location> locations;
		vector<BucketObject> bucketObjects;
		bool doWifi;
		bool doCopyFilesToDataPath;
	private:
		void saveFileToDisk(string sourceFile, string newFile);
};
