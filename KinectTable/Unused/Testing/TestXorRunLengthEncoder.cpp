//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "TestXorRunLengthEncoder.h"

// Project
#include "XorRunLength.h"

// Standard
#include <assert.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------



namespace Testing
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------

void ExportDepthImageToCsv(const char* filename, const DepthImage& depthImage);


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void TestXorRunLengthEncoder(DepthImage& depthImage)
{
	/*
	static DepthImage otherDepthImage;
	static ByteStream byteStream;
	static size_t length;
	memcpy(otherDepthImage.data, depthImage.data, sizeof(depthImage.data));
	Codecs::EncodeXorRunLength(depthImage, byteStream, length);
	//printf("Size: %u - %u\n", sizeof(depthImage), length);
	//printf("Perc: %f\n", length / (float)sizeof(depthImage));
	Codecs::DecodeXorRunLength(byteStream, length, depthImage);
	int error = memcmp(depthImage, otherDepthImage, sizeof(depthImage));
	if(error != 0)
	{
		
		//Write depth image data
		ExportDepthImageToCsv("error_original.csv", otherDepthImage);
		
		//Write depth image data
		ExportDepthImageToCsv("error_decoded.csv", depthImage);


		//Open file
		FILE* file = fopen("error_bytestream.csv", "w");
		if(file == NULL)
			throw errno;

		for(size_t i=0; i<length; i++)
		{
			fprintf(file, "%2X,", byteStream[i]);
		}

		//Close file
		(void)fclose(file);

		assert(false);
	}
	*/
	return;	
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------

static void ExportDepthImageToCsv(const char* filename, const DepthImage& depthImage)
{
	
	//Open file
	FILE* file = fopen(filename, "w");
	if(file == NULL)
		throw errno;

	//Write data
	for(int y=0; y<DEPTH_RES_Y; y++)
	{
		fprintf(file, "%hu", depthImage.data[y][0]);

		for(int x=1; x<DEPTH_RES_X; x++)
		{
			fprintf(file, ",%hu", depthImage.data[y][x]);
		}

		fprintf(file, "\n");
	}


	//Close file
	(void)fclose(file);

	return;
}


}