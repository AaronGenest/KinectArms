#ifndef SENSORS__KINECT_SENSOR_H
#define SENSORS__KINECT_SENSOR_H


// Module Check //
#ifndef MOD_SENSORS
#error "Files outside module cannot access file directly."
#endif


//
// These functions allow the user to grab images from a Kinect sensor.  The images are cleaned up before sent to the user.
//

#ifdef KINECT_MICROSOFT
	#include "KinectSensor_Microsoft.h"
#elif KINECT_OPEN_NI
	#include "KinectSensor_OpenNi.h"
#else
	#error "Please specify a Kinect SDK"
#endif

#endif