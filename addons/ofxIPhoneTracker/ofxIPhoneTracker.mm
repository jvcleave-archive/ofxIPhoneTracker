//
//  ofxIPhoneTracker.mm
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

#include "ofxIPhoneTracker.h"
#import "parsembdb.h"
#import "FMDatabase.h"



//--------------------------------------------------------------
ofxIPhoneTracker::ofxIPhoneTracker()
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	doCopyFilesToDataPath = false;
	doWifi = false;
}



void ofxIPhoneTracker::loadLocationDB()
{
	NSString* backupPath = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Application Support/MobileSync/Backup/"];
	
	
	
	NSLog(@"backupPath %@", backupPath);
	
	NSFileManager *fm = [NSFileManager defaultManager];
	NSArray* backupContents = [[NSFileManager defaultManager] directoryContentsAtPath:backupPath];
	
	NSMutableArray* fileInfoList = [NSMutableArray array];
	for (NSString *childName in backupContents) {
		NSString* childPath = [backupPath stringByAppendingPathComponent:childName];
		
		NSString *plistFile = [childPath   stringByAppendingPathComponent:@"Info.plist"];
		
		NSError* error;
		NSDictionary *childInfo = [fm attributesOfItemAtPath:childPath error:&error];
		
		NSDate* modificationDate = [childInfo objectForKey:@"NSFileModificationDate"];    
		
		NSDictionary* fileInfo = [NSDictionary dictionaryWithObjectsAndKeys: 
								  childPath, @"fileName", 
								  modificationDate, @"modificationDate", 
								  plistFile, @"plistFile", 
								  nil];
		[fileInfoList addObject: fileInfo];
		
	}
	
	NSSortDescriptor* sortDescriptor = [[[NSSortDescriptor alloc] initWithKey:@"modificationDate" ascending:NO] autorelease];
	[fileInfoList sortUsingDescriptors:[NSArray arrayWithObject:sortDescriptor]];
	
	BOOL loadWorked = NO;
	for (NSDictionary* fileInfo in fileInfoList) {
		@try {
			NSString* newestFolder = [fileInfo objectForKey:@"fileName"];
			NSString* plistFile = [fileInfo objectForKey:@"plistFile"];
			
			NSDictionary *plist = [NSDictionary dictionaryWithContentsOfFile:plistFile];
			if (plist==nil)
			{
				NSLog(@"No plist file found at '%@'", plistFile);
				//continue;
			}else {
				NSLog(@"plist file found at '%@'", plistFile);
			}

			NSString* deviceName = [plist objectForKey:@"Device Name"];
			NSLog(@"file = %@, device = %@", plistFile, deviceName);  
			
			NSDictionary* mbdb = [ParseMBDB getFileListForPath: newestFolder];
			if (mbdb==nil)
			{
				NSLog(@"No MBDB file found at '%@'", newestFolder);
				//continue;
			}else {
				NSLog(@"MBDB file found at '%@'", newestFolder);
			}

				
			NSString* wantedFileName = @"Library/Caches/locationd/consolidated.db";
			NSString* dbFileName = nil;
			for (NSNumber* offset in mbdb)
			{
				NSDictionary* fileInfo = [mbdb objectForKey:offset];
				NSString* fileName = [fileInfo objectForKey:@"filename"];
				if ([wantedFileName compare:fileName]==NSOrderedSame) 
				{
					dbFileName = [fileInfo objectForKey:@"fileID"];
				}
			}
			
			if (dbFileName==nil) {
				NSLog(@"No consolidated.db file found in '%@'", newestFolder);
				//continue;
			}else {
				NSLog(@"consolidated.db file found in '%@'", newestFolder);
			}

			
			NSString* dbFilePath = [newestFolder stringByAppendingPathComponent:dbFileName];
			const char * dbFilePathInChars = [dbFilePath UTF8String];
			const char * deviceNameInChars  = [deviceName UTF8String];
			loadWorked = tryToLoadLocationDB(dbFilePathInChars, deviceNameInChars);
			if (loadWorked)
			{
				if (doCopyFilesToDataPath)
				{
					
					saveFileToDisk((string)dbFilePathInChars, deviceNameInChars);
				}
				break;
			}
		}
		@catch (NSException *exception)
		{
			NSLog(@"Exception: %@", [exception reason]);
		}
	}
	
	if (!loadWorked)
	{
		NSLog(@"Couldn't load consolidated.db file from '%@'", backupPath);
		
	}else {
		NSLog(@"LOADED! %@ ", backupPath);
	}

}
void ofxIPhoneTracker::saveFileToDisk(string sourceFile, string newFile)
{
	const char * systemCommand;
	string systemCommandString = "cp \"";
	systemCommandString +=  sourceFile;
	systemCommandString += "\" \"" + ofToDataPath("", true) + newFile + "\"";
	
	systemCommand = systemCommandString.c_str();
	
	system(systemCommand);
}
bool ofxIPhoneTracker::tryToLoadLocationDB(const char * locationDBPath, const char * deviceName)
{
	NSLog(@"tryToLoadLocationDB");
	cout << "locationDBPath: " << locationDBPath << " deviceName: " << deviceName << endl;
    NSString *nsLocationDBPath = [NSString stringWithCString:locationDBPath];

	FMDatabase* database = [FMDatabase databaseWithPath: nsLocationDBPath];
	[database setLogsErrors: YES];
	BOOL openWorked = [database open];
	if (!openWorked) {
		return NO;
	}
	
	const float precision = 100;
	NSMutableDictionary* buckets = [NSMutableDictionary dictionary];
	
	NSString* queries[] = {@"SELECT * FROM CellLocation;", @"SELECT * FROM WifiLocation;"};
	
	// Temporarily disabled WiFi location pulling, since it's so dodgy. Change to 
	int numPasses = 1;
	if (doWifi) 
	{
		numPasses =  2;
	}
	for (int pass=0; pass<numPasses; pass+=1) {
		
		FMResultSet* results = [database executeQuery:queries[pass]];

		while ([results next]) {
			NSDictionary* row = [results resultDict];
			
			NSNumber* latitude_number = [row objectForKey:@"latitude"];
			NSNumber* longitude_number = [row objectForKey:@"longitude"];
			NSNumber* timestamp_number = [row objectForKey:@"timestamp"];
			
			const float latitude = [latitude_number floatValue];
			const float longitude = [longitude_number floatValue];
			const float timestamp = [timestamp_number floatValue];
			
			// The timestamps seem to be based off 2001-01-01 strangely, so convert to the 
			// standard Unix form using this offset
			const float iOSToUnixOffset = (31*365.25*24*60*60);
			const float unixTimestamp = (timestamp+iOSToUnixOffset);
			
			if ((latitude==0.0)&&(longitude==0.0)) {
				continue;
			}
			
			const float weekInSeconds = (7*24*60*60);
			const float timeBucket = (floor(unixTimestamp/weekInSeconds)*weekInSeconds);
			
			NSDate* timeBucketDate = [NSDate dateWithTimeIntervalSince1970:timeBucket];
			
			NSString* timeBucketString = [timeBucketDate descriptionWithCalendarFormat:@"%Y-%m-%d" timeZone:nil locale:nil];
			
			const float latitude_index = (floor(latitude*precision)/precision);  
			const float longitude_index = (floor(longitude*precision)/precision);
			
			BucketObject bucketObject;
			bucketObject.latitude = latitude;
			bucketObject.longitude = longitude;
			bucketObject.timestamp = timestamp;
			bucketObject.unixTimestamp = unixTimestamp;
			bucketObject.timeBucket = timeBucket;
			bucketObject.latitude_index = latitude_index;
			bucketObject.longitude_index = longitude_index;
			bucketObject.point = ofPoint(latitude, longitude);
			
			

			bucketObjects.push_back(bucketObject);
			
			NSString* allKey = [NSString stringWithFormat:@"%f,%f,All Time", latitude_index, longitude_index];
			NSString* timeKey = [NSString stringWithFormat:@"%f,%f,%@", latitude_index, longitude_index, timeBucketString];
			
			NSNumber* allExistingValue = [buckets objectForKey:allKey];
			if (allExistingValue==nil) {
				allExistingValue = [NSNumber numberWithInteger:0];
			}
			NSNumber* allNewValue = [NSNumber numberWithInteger:([allExistingValue integerValue]+1)];
			
			[buckets setObject: allNewValue forKey: allKey];
 			NSNumber* existingValue = [buckets objectForKey:timeKey];
			if (existingValue==nil) {
				existingValue = [NSNumber numberWithInteger:0];
			}
			NSNumber* newValue = [NSNumber numberWithInteger:([existingValue integerValue]+1)];
			
			[buckets setObject: newValue forKey: timeKey];
		}
	}
	
	NSMutableArray* csvArray = [[[NSMutableArray alloc] init] autorelease];
	
	[csvArray addObject: @"lat,lon,value,time\n"];
	
	for (NSString* key in buckets) {
		NSNumber* count = [buckets objectForKey:key];
		
		NSArray* parts = [key componentsSeparatedByString:@","];
		NSString* latitude_string = [parts objectAtIndex:0];
		NSString* longitude_string = [parts objectAtIndex:1];
		NSString* time_string = [parts objectAtIndex:2];
		Location location;
		location.count = [count integerValue];
		location.latitude_string = [latitude_string UTF8String];
		location.longitude_string = [longitude_string UTF8String];
		location.time_string = [time_string UTF8String];
		locations.push_back(location);
		NSString* rowString = [NSString stringWithFormat:@"%@,%@,%@,%@\n", latitude_string, longitude_string, count, time_string];
		[csvArray addObject: rowString];
	}
	
	if ([csvArray count]<10) {
		return NO;
	}
	
	NSString* csvText = [csvArray componentsJoinedByString:@"\n"];
	//NSLog(@"output %@", csvText);
	/*id scriptResult = [scriptObject callWebScriptMethod: @"storeLocationData" withArguments:[NSArray arrayWithObjects:csvText,deviceName,nil]];
	if(![scriptResult isMemberOfClass:[WebUndefined class]]) {
		NSLog(@"scriptResult='%@'", scriptResult);
	}*/
	
	return true;
}
