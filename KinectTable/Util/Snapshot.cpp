//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_UTIL

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Snapshot.h"

// Module
#include "DataExporter.h"
#include "Drawing.h"

// Project


// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Util
{



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void Snapshot::SetDirectoryPath(const Path& path)
{
	directoryPathLength = strlen(path);
	if(directoryPathLength >= 255)
		throw -1;

	strcpy(fullPath, path);
	return;
}


void Snapshot::TakeSnapshot(const char* filePath, const DataTypes::ColorImage& colorImage)
{

	if(frameNum != snapshotFrame)
		return;

	TakeSnapshotNow(filePath, colorImage);
	return;
}

void Snapshot::TakeSnapshot(const char* filePath, const DataTypes::DepthImage& depthImage)
{
	if(frameNum != snapshotFrame)
		return;
	
	TakeSnapshotNow(filePath, depthImage);
	return;
}

void Snapshot::TakeSnapshot(const char* filePath, const DataTypes::BinaryImage& binaryImage)
{
	if(frameNum != snapshotFrame)
		return;

	TakeSnapshotNow(filePath, binaryImage);
	return;
}


void Snapshot::TakeSnapshot(const char* filePath, const DataTypes::Types<float>::Image& floatImage)
{
	if(frameNum != snapshotFrame)
		return;

	TakeSnapshotNow(filePath, floatImage);
	return;
}



void Snapshot::TakeSnapshotNow(const char* filePath, const DataTypes::ColorImage& colorImage)
{
	if(!isEnabled)
		return;

	if(strlen(filePath) >= 255)
		throw -1;

	strcpy(&fullPath[directoryPathLength], filePath);
	DataExporter::ExportColorImageToBitmap(fullPath, colorImage);
	return;
}

void Snapshot::TakeSnapshotNow(const char* filePath, const DataTypes::DepthImage& depthImage)
{
	if(!isEnabled)
		return;

	static DataTypes::ColorImage colorImage;
	Drawing::ConvertToColor(depthImage, colorImage, true);

	TakeSnapshotNow(filePath, colorImage);
	return;
}

void Snapshot::TakeSnapshotNow(const char* filePath, const DataTypes::BinaryImage& binaryImage)
{
	if(!isEnabled)
		return;

	static DataTypes::ColorImage colorImage;
	Drawing::ConvertToColor(binaryImage, colorImage);

	TakeSnapshotNow(filePath, colorImage);
	return;
}


void Snapshot::TakeSnapshotNow(const char* filePath, const DataTypes::Types<float>::Image& floatImage)
{
	if(!isEnabled)
		return;

	static DataTypes::ColorImage colorImage;
	Drawing::ConvertToColor(floatImage, colorImage, true);

	TakeSnapshotNow(filePath, colorImage);
	return;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Private Static Fields
//---------------------------------------------------------------------------

int Snapshot::snapshotFrame = -1;
int Snapshot::frameNum = 0;
char Snapshot::fullPath[255*255] = "C:\\Temp\\";
int Snapshot::directoryPathLength = 0;

bool Snapshot::isEnabled = false;

}
