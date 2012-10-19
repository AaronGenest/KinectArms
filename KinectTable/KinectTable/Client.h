#ifndef KINECT_TABLE__CLIENT_H
#define KINECT_TABLE__CLIENT_H


//
// This is the KinectTable client interface.
//


// Includes //


// Project
#include "DataTypes/DataTypes.h"

// Standard C++


// Standard C




// Namespaces //




namespace KinectTable
{
	
	class Client
	{

	  // PUBLIC //
	  public:

		//!!bool IsConnected() const;
		
		// Get the current parameters for which data should be calculated
		virtual void GetParams(KinectDataParams& params) = 0;

		// Sets the current parameters for which data should be calculated
		virtual void SetParams(const KinectDataParams& params) = 0;

		virtual void RecalculateTableCorners() = 0;

		// Gets the current data from the Kinect
		// Returns whether the data is different from the last time it was grabbed.
		virtual bool GetData(KinectData& data, bool waitForNewData = false) = 0;

	};
	
}


#endif