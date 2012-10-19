//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_IMAGE_PROCESSING  //Allow access to module for testing purposes

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "HandDetectionTest.h"

// Project
#include "DataTypes/DataTypes.h"
#include "ImageProcessing/HandDetection.h"
#include "ImageProcessing/Helpers.h"
#include "Util/DataExporter.h"
#include "Util/Drawing.h"
#include "Util/Helpers.h"

// Standard C++


// Standard C
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------


namespace HandDetectionTest
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


int RunHandDetectionTest(char* imagePath)
{

	int ret = 0;

	printf("Starting hand detection test...\n\n");


	// Load image
	DepthImage depthImage;
	printf("Loading image: %s\n", imagePath); fflush(stdout);
	Util::DataExporter::ImportCsvToDepthImage(imagePath, depthImage);
	
	// Table corners
	Table table;
	table.depth = 1800;
	table.corners.push_back(Point2Di(103, 44));
	table.corners.push_back(Point2Di(580, 44));
	table.corners.push_back(Point2Di(580, 360));
	table.corners.push_back(Point2Di(103, 360));

	// Get table corners
	printf("Processing image...\n"); fflush(stdout);
	std::vector<Hand> hands;
	ImageProcessing::HandDetection::HandDetect(depthImage, table, hands);



	// Create color image
	ColorImage colorImage;
	memset(colorImage.data, 0, sizeof(colorImage.data));
	for(int i=0; i<hands.size(); i++)
	{
		int colorValue = (int)(255 * i / hands.size());
		ColorPixel color(colorValue, colorValue, colorValue);

		// Get hand blob
		static BinaryImage armBlob;
		hands[i].CreateArmBlob(armBlob);
		for(int y=0; y<armBlob.rows; y++)
		{
			for(int x=0; x<armBlob.cols; x++)
			{
				if(armBlob.data[y][x])
					colorImage.data[y][x] = color;
			}
		}
	}

	
	// Highlight border on image
	for(int i=0; i<hands.size(); i++)
		Util::Drawing::DrawPoints(colorImage, hands[i].boundary, ColorPixel::Blue);
	
	
	// Highlight finger tips on image
	const int markerSize = 5;
	for(int i=0; i<hands.size(); i++)
		Util::Drawing::DrawPoints(colorImage, hands[i].fingerTips, ColorPixel::Green, markerSize);
	
	// Highlight finger bases on image
	for(int i=0; i<hands.size(); i++)
		Util::Drawing::DrawPoints(colorImage, hands[i].fingerBases, ColorPixel::Red, markerSize);


	// Show image
	Util::Helpers::ShowImage(colorImage);

	// Export image
	/*const char outputImagePath[] = "C:\\Users\\Mike\\Desktop\\image.bmp";
	printf("Exporting image: %s\n", outputImagePath); fflush(stdout);
	DataExporter::ExportColorImageToBitmap(outputImagePath, colorImage);*/

	return ret;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------



}
