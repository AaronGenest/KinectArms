#ifdef KINECT_OPEN_NI

//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SENSORS

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "KinectSensor_OpenNi.h"

// Project
#include "Util/Helpers.h"

// OpenNI
#include <XnCppWrapper.h>

// OpenCV
#include <opencv2\opencv.hpp>


// Standard C++
#include <exception>

// Standard C
#include <Windows.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace xn;


namespace KinectSensor
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

// OpenNI
#define SAMPLE_XML_PATH "Data\\SamplesConfig.xml"

Context deviceContext;
ImageGenerator colorImageGenerator;
DepthGenerator depthImageGenerator;



//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------

void GetColorAndDepthImages(ColorImage& colorImage, DepthImage& depthImage);
void GetValidPixelMap(const DepthImage& depthImage, BinaryImage& validityImage);
void PreProcessDepthData(const DepthImage& rawDepth, DepthImage& processedDepth);



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------



int Init()
{
	
	XnStatus rc;

	//Make sure our image types are the same as the OpenNI image types.
	assert(sizeof(XnRGB24Pixel) == sizeof(ColorPixel));
	assert(sizeof(XnDepthPixel) == sizeof(DepthPixel));
	assert(sizeof(XnStatus) == sizeof(int));

	// Load OpenNI xml settings
	char filePath[255];
	int length = Util::Helpers::GetExeDirectory(filePath, sizeof(filePath));
	filePath[length] = '\\';
	strcpy(&filePath[length+1], SAMPLE_XML_PATH);

	EnumerationErrors errors;
	rc = deviceContext.InitFromXmlFile(filePath, &errors);
	if (rc == XN_STATUS_NO_NODE_PRESENT)
	{
		//One reason would be if Microsoft SDK is installed beside PrimeSense.  Device manager should say PrimeSense instead of Microsoft Kinect.
		
		//XnChar strError[1024];
		//errors.ToString(strError, 1024);
		//LOGE("%s\n", strError);
		return -1;
	}
	else if (rc != XN_STATUS_OK)
	{
		fprintf(stderr, "%s\n", xnGetStatusString(rc));
		/*LOGE("Open failed: %s\n", xnGetStatusString(rc));*/
		return (rc);
	}

	// Retrieve colour and depth nodes
	rc = deviceContext.FindExistingNode(XN_NODE_TYPE_IMAGE, colorImageGenerator);
	rc = deviceContext.FindExistingNode(XN_NODE_TYPE_DEPTH, depthImageGenerator);

	// Set mirror mode to off
	SetMirrorMode(false);

	// Get a frame to perform checks on it
	ImageMetaData colorImageMetaData;
	DepthMetaData depthImageMetaData;
	depthImageGenerator.GetMetaData(depthImageMetaData);
	colorImageGenerator.GetMetaData(colorImageMetaData);

	// Hybrid mode isn't supported in this sample
	if (colorImageMetaData.FullXRes() != depthImageMetaData.FullXRes() || colorImageMetaData.FullYRes() != depthImageMetaData.FullYRes())
	{
		/*LOGE("The device depth and image resolution must be equal!\n");*/
		return 1;
	}

	// RGB is the only image format supported.
	if (colorImageMetaData.PixelFormat() != XN_PIXEL_FORMAT_RGB24)
	{
		/*LOGE("The device image format must be RGB24\n");*/
		return 1;
	}
	
	// Need to make sure the automatic alignment of colour and depth images is supported.
	XnBool isSupported = depthImageGenerator.IsCapabilitySupported("AlternativeViewPoint");
	if(!isSupported)
	{
		/*LOGE("Cannot set AlternativeViewPoint!\n");*/
		return 1;
	}

	
	// Set it to VGA maps at 30 FPS
	/*XnMapOutputMode mapMode;
	mapMode.nXRes = XN_VGA_X_RES;
	mapMode.nYRes = XN_VGA_Y_RES;
	mapMode.nFPS = 60;
	rc = g_depth.SetMapOutputMode(mapMode);
	if(rc)
	{
		LOGE("Failed to set depth map mode: %s\n", xnGetStatusString(rc));
		return 1;
	}
	mapMode.nFPS = 30;
	rc = g_image.SetMapOutputMode(mapMode);
	if(rc)
	{
		LOGE("Failed to set image map mode: %s\n", xnGetStatusString(rc));
		return 1;
	}*/


	// Set automatic alignment of the colour and depth images.
	rc = depthImageGenerator.GetAlternativeViewPointCap().SetViewPoint(colorImageGenerator);
	if(rc)
	{
		/*LOGE("Failed to set depth map mode: %s\n", xnGetStatusString(rc));*/
		return 1;
	}


	return XN_STATUS_OK;
}


void DeInit()
{

	//deviceContext.Shutdown();
	return;
}


//NOTE: Based on tests, data range seems to be valid between 511 (0x1FF) and 8191 (0x1FFF).
void GetData(ColorImage& colorImage, DepthImage& depthImage, BinaryImage& validityImage)
{
	
	static DepthImage rawDepthImage;

	
	//Get the colour and depth data
	//!!GetColorAndDepthImages(colorImage, rawDepthImage);
	GetColorAndDepthImages(colorImage, depthImage);
	
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
	return (deviceContext.GetGlobalMirror() == 0) ? false : true;
}


void SetMirrorMode(bool mirrorMode)
{
	XnStatus rc = deviceContext.SetGlobalMirror(mirrorMode);
	if(rc != XN_STATUS_OK)
		throw rc;

	return;
}




//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


// Gets the colour and depth data from the Kinect sensor.
void GetColorAndDepthImages(ColorImage& colorImage, DepthImage& depthImage)
{

	XnStatus rc = XN_STATUS_OK;
	
	// Read a new frame, blocking operation
	rc = deviceContext.WaitAnyUpdateAll();
	if (rc != XN_STATUS_OK)
	{
		/*LOGE("Read failed: %s\n", xnGetStatusString(rc));*/
		throw rc;
	}

	
	// Get handles to new data
	static ImageMetaData colorImageMetaData;
	static DepthMetaData depthImageMetaData;
	colorImageGenerator.GetMetaData(colorImageMetaData);
	depthImageGenerator.GetMetaData(depthImageMetaData);

	
	// Validate images
	if (!depthImageGenerator.IsValid() || !colorImageGenerator.IsValid())
	{
		/*LOGE("Error: Color or depth image is invalid.");*/
		throw 1;
	}


	// Fetch pointers to data
	const XnRGB24Pixel* pColorImage = colorImageMetaData.RGB24Data(); //g_depth.GetRGB24ImageMap()
	const XnDepthPixel* pDepthImage = depthImageMetaData.Data();// g_depth.GetDepthMap();
	
	
	// Copy data over to arrays
	memcpy(colorImage.data, pColorImage, sizeof(colorImage.data));
	memcpy(depthImage.data, pDepthImage, sizeof(depthImage.data));
	
	colorImage.rows = colorImage.maxRows;
	colorImage.cols = colorImage.maxCols;

	depthImage.rows = depthImage.maxRows;
	depthImage.cols = depthImage.maxCols;
	
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
			validityData[y][x] = depthData[y][x] != XN_DEPTH_NO_SAMPLE_VALUE;
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

#endif //KINECT_OPEN_NI