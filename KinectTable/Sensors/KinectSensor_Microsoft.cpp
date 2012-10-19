#ifdef KINECT_MICROSOFT
//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SENSORS

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "KinectSensor_Microsoft.h"

// Project


// Micrsoft Kinect For Windows
#include <Windows.h>
#include <NuiApi.h>

// OpenCV
#include <opencv2\opencv.hpp>


// Standard C++
#include <exception>

// Standard C
#include <assert.h>




//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------



namespace KinectSensor
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

const DepthPixel invalidDepthValue = 0;

INuiSensor* sensor = NULL;
HANDLE colorImageReadyEvent;
HANDLE depthImageReadyEvent;
HANDLE colorStream;
HANDLE depthStream;

LARGE_INTEGER colorTimeStamp;
LARGE_INTEGER depthTimeStamp;

unsigned short depthData[640*480];
long colorCoordinates[640*480*2];


typedef struct _RgbPixel
{

	const unsigned char& red() const { return (unsigned char&)bytes[2]; };
	const unsigned char& green() const { return (unsigned char&)bytes[1]; };
	const unsigned char& blue() const { return (unsigned char&)bytes[0]; };

	unsigned char& red() { return (unsigned char&)bytes[2]; };
	unsigned char& green() { return (unsigned char&)bytes[1]; };
	unsigned char& blue() { return (unsigned char&)bytes[0]; };

  private:
	char bytes[4];

} RgbPixel;

//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------

void ConvertDepthFrameToArray(const NUI_IMAGE_FRAME& frame, DataTypes::DepthImage& image);
void ConvertColorFrameToArray(const NUI_IMAGE_FRAME& frame, DataTypes::ColorImage& image);
void MapColorToDepth(const DataTypes::DepthImage& depthImage, const DataTypes::ColorImage& colorImage, DataTypes::ColorImage& alignedColorImage);

void GetColorAndDepthImages(ColorImage& colorImage, DepthImage& depthImage);
void GetValidPixelMap(const DepthImage& depthImage, BinaryImage& validityImage);
void PreProcessDepthData(const DepthImage& rawDepth, DepthImage& processedDepth);



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------



int Init()
{

	//Make sure our image types are the same as the OpenNI image types.
	//assert(sizeof(XnRGB24Pixel) == sizeof(ColorPixel));
	//assert(sizeof(XnDepthPixel) == sizeof(DepthPixel));
	//assert(sizeof(XnStatus) == sizeof(int));
	
	INuiSensor* nuiSensorPtr;
	int ret;

	// Get number of sensors
	int sensorCount = 0;
	ret = NuiGetSensorCount(&sensorCount);
	if(ret < 0)
	{
		return -1;
	}

	// Connect to first sensor
	for(int i=0; i<sensorCount; i++)
	{
		// Create sensor so we can check the status
		ret = NuiCreateSensorByIndex(i, &nuiSensorPtr);
		if(ret < 0)
			continue;

		// Get the status of the sensor
		ret = nuiSensorPtr->NuiStatus();
		if(ret < 0)
		{
			(void)nuiSensorPtr->Release();
			nuiSensorPtr = NULL;
			continue;
		}
		
		sensor = nuiSensorPtr;
	}

	// Make sure we have a sensor
	if(sensor == NULL)
	{
		return -1;
	}
	
	// Initialize Kinect
	ret = sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH);
	if(ret < 0)
	{
		return -1;
	}
	
	// Create an event that will be signaled when color data is available
    colorImageReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Create an event that will be signaled when depth data is available
    depthImageReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Open a color image stream to receive color frames
	ret = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, colorImageReadyEvent, &colorStream);
	if(ret < 0)
	{
		return -1;
	}

	// Open a depth image stream to receive depth frames
    ret = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, 0, 2, depthImageReadyEvent, &depthStream);
	if(ret < 0)
	{
		return -1;
	}

	return 0;
}


void DeInit()
{

	if(sensor != NULL)
	{
		sensor->NuiShutdown();
		sensor = NULL;
	}

	CloseHandle(colorImageReadyEvent);
	CloseHandle(depthImageReadyEvent);
	CloseHandle(colorStream);
	CloseHandle(depthStream);

	return;
}


//NOTE: Based on tests, data range seems to be valid between 511 (0x1FF) and 8191 (0x1FFF).
void GetData(ColorImage& colorImage, DepthImage& depthImage, BinaryImage& validityImage)
{
	
	static DepthImage rawDepthImage;

	
	//Get the colour and depth data
	static DataTypes::ColorImage unalignedColorImage;
	//!!GetColorAndDepthImages(colorImage, rawDepthImage);
	GetColorAndDepthImages(unalignedColorImage, depthImage);
	
	// Align the color and depth images
	MapColorToDepth(depthImage, unalignedColorImage, colorImage);

	// Get an image which specifies the valid depth pixels
	//!!GetValidPixelMap(rawDepthImage, validityImage);
	GetValidPixelMap(depthImage, validityImage);
	
	// Process the depth data
	//!!PreProcessDepthData(rawDepthImage, depthImage);
	PreProcessDepthData(depthImage, depthImage);
	
	return;
}



bool GetMirrorMode()
{
	throw ENOTSUP;
}


void SetMirrorMode(bool mirrorMode)
{
	throw ENOTSUP;
}




//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


// Gets the colour and depth data from the Kinect sensor.
void GetColorAndDepthImages(ColorImage& colorImage, DepthImage& depthImage)
{
	int ret;

	// Make sure we have a sensor
	if(sensor == NULL)
		throw -1;

	bool needToMapColorToDepth = false;

	if ( WAIT_OBJECT_0 == WaitForSingleObject(depthImageReadyEvent, 0) )
    {
		
		// Attempt to get the color frame
		NUI_IMAGE_FRAME imageFrame;
		ret = sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame);
		if (ret < 0)
		{
			throw -1;
		}

		depthTimeStamp = imageFrame.liTimeStamp;
		ConvertDepthFrameToArray(imageFrame, depthImage);
		
		// Release the frame
		sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);

		needToMapColorToDepth = true;
	}

	if ( WAIT_OBJECT_0 == WaitForSingleObject(colorImageReadyEvent, 0) )
    {
		
		// Attempt to get the color frame
		NUI_IMAGE_FRAME imageFrame;
		ret = sensor->NuiImageStreamGetNextFrame(colorStream, 0, &imageFrame);
		if (ret < 0)
		{
			throw -1;
		}

		colorTimeStamp = imageFrame.liTimeStamp;
		ConvertColorFrameToArray(imageFrame, colorImage);
		
		// Release the frame
		sensor->NuiImageStreamReleaseFrame(colorStream, &imageFrame);

		needToMapColorToDepth = true;
	}
	
	// Depth is 30 fps.  For any given combination of FPS, we should ensure we are within half a frame of the more frequent of the two.  
    // But depth is always the greater (or equal) of the two, so just use depth FPS.
    const int depthFps = 30;
    const int halfADepthFrameMs = (1000 / depthFps) / 2;

	// If the color frame is more than half a depth frame ahead of the depth frame we have,
    // then we should wait for another depth frame.  Otherwise, just go with what we have.
    if (colorTimeStamp.QuadPart - depthTimeStamp.QuadPart > halfADepthFrameMs)
    {
        needToMapColorToDepth = false;
    }

	
	return;
}


void ConvertDepthFrameToArray(const NUI_IMAGE_FRAME& frame, DataTypes::DepthImage& image)
{

	assert(frame.eImageType == NUI_IMAGE_TYPE_DEPTH);
	assert(frame.eResolution == NUI_IMAGE_RESOLUTION_640x480);
	INuiFrameTexture* texture = frame.pFrameTexture;
	NUI_LOCKED_RECT lockedRect;
		
	// Lock the frame data so the Kinect knows not to modify it while we're reading it
	texture->LockRect(0, &lockedRect, NULL, 0);

	{
		// Make sure we've received valid data
		if (lockedRect.Pitch == 0)
		{
			texture->UnlockRect(0);
			throw -1;
		}


		//////////////////////////////

		assert((lockedRect.size % sizeof(short)) == 0);
		assert((lockedRect.Pitch % sizeof(short)) == 0);
		int cols = lockedRect.Pitch / sizeof(short);
		int rows = lockedRect.size / lockedRect.Pitch;


		memcpy(depthData, lockedRect.pBits, lockedRect.size);

		int byteNum = 0;
		for(int y=0; y<rows; y++)
		{
			for(int x=0; x<cols; x++)
			{
			
				const unsigned short& pixel = (unsigned short&)lockedRect.pBits[byteNum];
				DepthPixel& depth = image.data[y][x];
			
				// discard the portion of the depth that contains only the player index
				depth = NuiDepthPixelToDepth(pixel);

				byteNum += sizeof(short);
			}
		}

		image.rows = rows;
		image.cols = cols;

		//////////////////////////////


	}

	// We're done with the texture so unlock it
	texture->UnlockRect(0);
	
	return;
}

void ConvertColorFrameToArray(const NUI_IMAGE_FRAME& frame, DataTypes::ColorImage& image)
{

	assert(frame.eImageType == NUI_IMAGE_TYPE_COLOR);
	assert(frame.eResolution == NUI_IMAGE_RESOLUTION_640x480);
	INuiFrameTexture* texture = frame.pFrameTexture;
	NUI_LOCKED_RECT lockedRect;
		
	// Lock the frame data so the Kinect knows not to modify it while we're reading it
	texture->LockRect(0, &lockedRect, NULL, 0);

	{
		// Make sure we've received valid data
		if (lockedRect.Pitch == 0)
		{
			texture->UnlockRect(0);
			throw -1;
		}

	
		//////////////////////////////

		assert((lockedRect.size % 4) == 0);
		assert((lockedRect.Pitch % 4) == 0);
		int cols = lockedRect.Pitch / 4;
		int rows = lockedRect.size / lockedRect.Pitch;


		int byteNum = 0;
		for(int y=0; y<rows; y++)
		{
			for(int x=0; x<cols; x++)
			{
			
				const RgbPixel& pixel = (RgbPixel&)lockedRect.pBits[byteNum];
				ColorPixel& color = image.data[y][x];
			
				color.red = pixel.red();
				color.green = pixel.green();
				color.blue = pixel.blue();
			
				byteNum += 4;
			}
		}

		image.rows = rows;
		image.cols = cols;

		//////////////////////////////

	}

	// We're done with the texture so unlock it
	texture->UnlockRect(0);
	
	return;
}


void MapColorToDepth(const DataTypes::DepthImage& depthImage, const DataTypes::ColorImage& colorImage, DataTypes::ColorImage& alignedColorImage)
{
	

	// Get of x, y coordinates for color in depth space
    // This will allow us to later compensate for the differences in location, angle, etc between the depth and color cameras
    sensor->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
        NUI_IMAGE_RESOLUTION_640x480,
        NUI_IMAGE_RESOLUTION_640x480,
        640*480,
        depthData,
        640*480*2,
        colorCoordinates
        );

	const long colorToDepthDivisor = COLOR_RES_X / DEPTH_RES_X;

	// loop over each row and column of the color
	for (long y = 0; y < COLOR_RES_Y; ++y)
    {
        for (long x = 0; x < COLOR_RES_X; ++x)
        {
            // calculate index into depth array
            int depthIndex = x/colorToDepthDivisor + y/colorToDepthDivisor * DEPTH_RES_X;

            // retrieve the depth to color mapping for the current depth pixel
            LONG colorInDepthX = colorCoordinates[depthIndex * 2];
            LONG colorInDepthY = colorCoordinates[depthIndex * 2 + 1];

            // make sure the depth pixel maps to a valid point in color space
            if ( colorInDepthX >= 0 && colorInDepthX < COLOR_RES_X && colorInDepthY >= 0 && colorInDepthY < COLOR_RES_Y )
            {
                // set source for copy to the color pixel
				alignedColorImage.data[y][x] = colorImage.data[colorInDepthY][colorInDepthX];
            }
            else
            {
                alignedColorImage.data[y][x] = ColorPixel(ColorPixel::Black);
            }

        }
    }
	
	alignedColorImage.rows = colorImage.rows;
	alignedColorImage.cols = colorImage.cols;

	return;
}

// Gets a binary image of valid pixels in depth map.  True corresponds to valid pixels.
void GetValidPixelMap(const DepthImage& depthImage, BinaryImage& validityImage)
{
	
	const DepthImage::ArrayType& depthData = depthImage.data;
	BinaryImage::ArrayType& validityData = validityImage.data;

	// Loop through all pixels and mark whether they are valid or not
	for(size_t y=0; y<depthImage.rows; y++)
	{
		for(size_t x=0; x<depthImage.cols; x++)
		{
			validityData[y][x] = depthData[y][x] != invalidDepthValue;
		}
	}
	
	validityImage.rows = depthImage.rows;
	validityImage.cols = depthImage.cols;
	
	return;
}


// Cleans up the depth data
void PreProcessDepthData(const DepthImage& rawDepth, DepthImage& processedDepth)
{
	// Constants
	const int medianBoxSize = 5;


	// Create filtering matrices
	// quick opencv structs (does not copy! so ultra fast)
	cv::Mat input(rawDepth.maxRows, rawDepth.maxCols, CV_16U, (void*)rawDepth.data);
	cv::Mat output(processedDepth.maxRows, processedDepth.maxCols, CV_16U, processedDepth.data);

	
	// Fill all the invalid pixels
	{
		// simple blur
		//cv::blur(src, dest, cv::Size(3,3));

		// box filter
		//cv::boxFilter(src, dest, -1, cv::Size(5, 5), cv::Point(-1, -1), true);
		
		cv::medianBlur(input, output, medianBoxSize);

		// median blur
		//cv::Mat foo(dest);
		//cv::medianBlur(dest, foo, 5);

		//cv::GaussianBlur(src, dest, cv::Size(3,3), 3);
	}

	
	return;
}

}

#endif //KINECT_MICROSOFT