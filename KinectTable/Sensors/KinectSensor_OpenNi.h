#ifndef SENSORS__KINECT_SENSOR_OPEN_NI_H
#define SENSORS__KINECT_SENSOR_OPEN_NI_H


// Module Check //
#ifndef MOD_SENSORS
#error "Files outside module cannot access file directly."
#endif


//
// These functions allow the user to grab images from a Kinect sensor.  The images are cleaned up before sent to the user.
//


// Includes //
#include "DataTypes/DataTypes.h"



// Namespaces //

//!!Should be a singleton until we support multiple kinects!
//!!What happens when kinect disconnects?
namespace KinectSensor
{
	
	

	// Methods //

	// Initializes the device.
	int Init();

	// Deinitializes the device.
	void DeInit();


	// Gets the image and depth maps as well as a map of which pixels were valid from the depth sensor.
	void GetData(ColorImage& colorImage, DepthImage& depthImage, BinaryImage& validityImage);


	// Gets whether in mirror mode.  True means we are in mirror mode.
	bool GetMirrorMode();

	// Sets whether to be in mirror mode.  True specifies mirror mode.
	void SetMirrorMode(bool mirrorMode);

	
	//int GetColorResX() { return COLOR_RES_X; } //g_imageMD.FullXRes();
	//int GetColorResY() { return COLOR_RES_Y; } //g_imageMD.FullYRes();

	//int GetDepthResX() { return DEPTH_RES_X; } //g_depthMD.FullXRes();
	//int GetDepthResY() { return DEPTH_RES_Y; } //g_depthMD.FullYRes();

	
}


#endif