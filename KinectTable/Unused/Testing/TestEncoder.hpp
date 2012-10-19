//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "TestEncoder.h"

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

template<typename T>
void TestEncoder(typename const Types<T>::Image& image)
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


void OtherTestingStuff()
{
	//Get the channels of colors
	//static GrayImage redChannel;
	//static GrayImage greenChannel;
	//static GrayImage blueChannel;

	//Util::GetColorChannels(data.colorImage, redChannel, greenChannel, blueChannel);

	//static Types<GrayPixel>::FlatImageBytes flattenedImageBytes;
	//Codecs::LibJpegEncoder::Encode<GrayPixel>(redChannel, flattenedImageBytes);
	//Codecs::LibJpegEncoder::Decode<DepthPixel>(flattenedImageBytes, codecFlattenedImage);


	/*
	data.depthImage.rows = 2;
	data.depthImage.cols = 2;
	data.depthImage.data[0][0] = 5;
	data.depthImage.data[0][1] = 5;
	data.depthImage.data[1][0] = 5;
	data.depthImage.data[1][1] = 5;
	*/

	/*
	//Get the channels of colors
	static GrayImage redChannel;
	static GrayImage greenChannel;
	static GrayImage blueChannel;

	Util::GetColorChannels(data.colorImage, redChannel, greenChannel, blueChannel);

	// We need to keep a value as the invalid value
	for(size_t y=0; y<data.colorImage.rows; y++)
	{
		for(size_t x=0; x<data.colorImage.cols; x++)
		{
			redChannel.data[y][x] = redChannel.data[y][x] == 0 ? 1 : redChannel.data[y][x];
		}
	}
	*/

	//
	/*
	static Codecs::XorDiff<DepthPixel> encoder(-1);
	static Types<DepthPixel>::FlatImage inputFlatDepthImage;
	static Types<DepthPixel>::FlatImage outputFlatDepthImage;
	data.depthImage.Flatten(inputFlatDepthImage);
	encoder.Next(inputFlatDepthImage, outputFlatDepthImage);
	memset(data.depthImage.data, 0, sizeof(data.depthImage.data));
	data.depthImage.Copy(outputFlatDepthImage, data.depthImage.rows, data.depthImage.cols);
	*/

	/*
	static Codecs::XorDiff<GrayPixel> redEncoder(0);
	static Types<GrayPixel>::FlatImage inputFlatDepthImage;
	static Types<GrayPixel>::FlatImage outputFlatDepthImage;
	redChannel.Flatten(inputFlatDepthImage);
	redEncoder.Next(inputFlatDepthImage, outputFlatDepthImage);
	redChannel.Copy(outputFlatDepthImage, redChannel.rows, redChannel.cols);

	memset(data.colorImage.data, 0, sizeof(data.colorImage.data));
	for(size_t y=0; y<data.colorImage.rows; y++)
	{
		for(size_t x=0; x<data.colorImage.cols; x++)
		{
			ColorPixel& pixel = data.colorImage.data[y][x];
			pixel.red = redChannel.data[y][x];
			//greenChannel.data[y][x] = pixel.green;
			//blueChannel.data[y][x] = pixel.blue;
		}
	}
	*/

	//
	/*
	typedef DepthPixel DataType;
	Types<DataType>::Image& testImage = data.depthImage;
	const size_t testSizeDownSample = 1;


	static Types<DataType>::FlatImage origFlattenedImage;
	static Types<DataType>::FlatImage codecFlattenedImage;
	testImage.Flatten(origFlattenedImage);
	testImage.Flatten(codecFlattenedImage);
				
	static Types<DataType>::FlatImageBytes flattenedImageBytes;
	codecFlattenedImage.size = codecFlattenedImage.size/testSizeDownSample;
	Codecs::ZlibEncoder::Encode<DataType>(codecFlattenedImage, flattenedImageBytes);
	Codecs::ZlibEncoder::Decode<DataType>(flattenedImageBytes, codecFlattenedImage);

	int origBytes = codecFlattenedImage.size * codecFlattenedImage.typeSize;
	int compressedBytes = flattenedImageBytes.size;
	printf("Size: %u - %u\n", origBytes, compressedBytes);
	printf("Perc: %f\n", compressedBytes / (float)origBytes);

	codecFlattenedImage.size = codecFlattenedImage.size * testSizeDownSample;
	assert(origFlattenedImage.size == codecFlattenedImage.size);

	int ret = memcmp(origFlattenedImage.data, codecFlattenedImage.data, origFlattenedImage.size * origFlattenedImage.typeSize);
	assert(ret == 0);

	memset(testImage.data, 0, sizeof(testImage.data));
	testImage.Copy(codecFlattenedImage, testImage.rows, testImage.cols);
	*/
}

}