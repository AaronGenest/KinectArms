//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_IMAGE_PROCESSING  //Allow access to module for testing purposes

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "TableDetectionTest.h"

// Project
#include "DataTypes/DataTypes.h"
#include "ImageProcessing/TableDetection.h"
#include "Util/DataExporter.h"
#include "Util/Drawing.h"
#include "Util/Helpers.h"

// Standard C++


// Standard C
#include <stdio.h>
#include <assert.h>
#include <time.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------



namespace TableDetectionTest
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


int RunTableDetectionTest(char* imagePath)
{

	int ret = 0;

	printf("Starting hand detection test...\n\n");

	/*
	DepthImage image;
	image.rows = 324;
	image.cols = 276;
	for(int y=0; y<image.rows; y++)
	{
		for(int x=0; x<image.cols; x++)
		{
			image.data[y][x] = y*x;
		}
	}
	

	DataExporter::ExportDepthImageToCsv("C:\\Users\\Mike\\Desktop\\Blah.csv", image);
	
	DepthImage otherImage;
	DataExporter::ImportCsvToDepthImage("C:\\Users\\Mike\\Desktop\\Blah.csv", otherImage);

	bool success = true;
	success &= otherImage.rows == image.rows;
	success &= otherImage.cols == image.cols;
	for(int y=0; y<image.rows; y++)
	{
		for(int x=0; x<image.cols; x++)
		{
			success &= otherImage.data[y][x] == image.data[y][x];
		}
	}

	if(success)
		printf("Test was successful");
	else
		printf("Test was NOT successful");
	*/

	// Load image
	DepthImage depthImage;
	printf("Loading image: %s\n", imagePath); fflush(stdout);
	Util::DataExporter::ImportCsvToDepthImage(imagePath, depthImage);


	// Get table corners
	printf("Processing image...\n"); fflush(stdout);
	BinaryImage testImage;
	std::vector<Point2Di> boundary;
	Table table;
	ImageProcessing::TableDetection::TableDetect(depthImage, testImage, boundary, table);



	// Create color image
	ColorImage colorImage;
	Util::Drawing::ConvertToColor(testImage, colorImage);
	

	// Highlight border on image
	const ColorPixel red(255, 0, 0);
	Util::Drawing::DrawPoints(colorImage, boundary, red);
	

	// Highlight corners on image
	const int markerSize = 5;
	const ColorPixel green(0, 255, 0);
	Util::Drawing::DrawPoints(colorImage, table.corners, green, markerSize);
	
	// Show image
	Util::Helpers::ShowImage(colorImage);

	return ret;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------



}
