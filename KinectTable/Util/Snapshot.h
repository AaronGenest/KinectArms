#ifndef UTIL__SNAPSHOT_H
#define UTIL__SNAPSHOT_H


// Module Check //
//#ifndef MOD_UTIL
//#error "Files outside module cannot access file directly."
//#endif


//
// This is a template.
//


// Includes //

// Module
#include "DataTypes/Images.h"

// Project


// Standard C++


// Standard C
#include <string.h>



// Namespaces //




namespace Util
{
	
	class Snapshot
	{

		typedef char (&Path)[255];
		//typedef char (&LongPath)[255*255];

	  // PUBLIC //
	  public:

		// Will use absolute paths if no path specified.
		static void SetDirectoryPath(const Path& path);

		static void SetSnapshotFrame(int frameNum) { snapshotFrame = frameNum; }
		static void Reset() { frameNum = 0; }
		static void NextFrame() { frameNum++; }
		static void SetEnable(bool enable) { isEnabled = enable; }

		static void TakeSnapshot(const char* filePath, const DataTypes::ColorImage& colorImage);
		static void TakeSnapshot(const char* filePath, const DataTypes::DepthImage& depthImage);
		static void TakeSnapshot(const char* filePath, const DataTypes::BinaryImage& binaryImage);

		static void TakeSnapshot(const char* filePath, const DataTypes::Types<float>::Image& floatImage);

		static void TakeSnapshotNow(const char* filePath, const DataTypes::ColorImage& colorImage);
		static void TakeSnapshotNow(const char* filePath, const DataTypes::DepthImage& depthImage);
		static void TakeSnapshotNow(const char* filePath, const DataTypes::BinaryImage& binaryImage);

		static void TakeSnapshotNow(const char* filePath, const DataTypes::Types<float>::Image& floatImage);

	  // PRIVATE //
	  private:

		Snapshot() { return; }
		static int snapshotFrame;
		static int frameNum;

		static char fullPath[255*255];
		static int directoryPathLength;

		static bool isEnabled;
	};

	
}


#endif