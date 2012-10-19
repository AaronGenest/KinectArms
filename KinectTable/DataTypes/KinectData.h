#ifndef DATA_TYPES__KINECT_DATA_H
#define DATA_TYPES__KINECT_DATA_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif


//
// This holds all the image pixel types.
//


// Includes //


// Module
#include "Table.h"
#include "KinectDataParams.h"
#include "Images.h"
#include "Points.h"
#include "Other.h"
#include "Hand.h"

// Project

// Standard C++
#include <vector>

// Standard C
#include <stdlib.h>


// Namespaces //




namespace DataTypes
{
	
	typedef struct __declspec(dllexport) tagKinectData
	{
		// Data params
		KinectDataParams available;

		// Raw Images
		ColorImage colorImage;
		DepthImage depthImage;
		BinaryImage validityImage;

		// Test Image
		ColorImage testImage;

		// Table Data
		Table table;

		// Arms Data
		std::vector<Hand> hands;

		size_t SizeOf() const
		{
			return available.SizeOf() + colorImage.SizeOf() + depthImage.SizeOf() + validityImage.SizeOf() + testImage.SizeOf();
		}

		//!! CopyTo is just the assignment operator!
		void CopyTo(tagKinectData& data) const;
		

	} KinectData;


}


#endif