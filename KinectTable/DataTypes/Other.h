#ifndef DATA_TYPES__OTHER_H
#define DATA_TYPES__OTHER_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif


//
// This holds all the image pixel types.
//


// Includes //


// Project
#include "Points.h"

// Standard C++

// Standard C



// Namespaces //




namespace DataTypes
{
	
	
typedef int Errno;






//const int MAX_BYTES_PER_STREAM = DEPTH_RES_X*DEPTH_RES_Y*sizeof(DepthPixel);





//The type of session connection.
enum SessionType
{
	SESSION_TYPE_PUBLIC,
	SESSION_TYPE_PRIVATE
};







//Specifies parameters for the type of connection to make
typedef struct tagSessionParameters
{
	tagSessionParameters() : dataParams() { return; }
	tagSessionParameters(KinectDataParams::EnableType enableType) : dataParams(enableType) { return; }

	KinectDataParams dataParams;

} SessionParameters;




typedef Point2Di Rectangle2Di[4];
//typedef Point3Df Rectangle3Df[4];




struct SubscriptionState
{

	typedef enum
	{

		Subscribed,
		Pending,
		Unsubscribed

	} Type;


	private: SubscriptionState() {}
};



}


#endif