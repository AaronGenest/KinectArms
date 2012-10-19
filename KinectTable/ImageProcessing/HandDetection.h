#ifndef IMAGE_PROCESSING__HAND_DETECTION_H
#define IMAGE_PROCESSING__HAND_DETECTION_H

// Module Check //
#ifndef MOD_IMAGE_PROCESSING
#error "Files outside module cannot access file directly."
#endif

//
// These functions detect tables in images.
//


// Includes //

//Project
#include "DataTypes/Images.h"
#include "DataTypes/Table.h"
#include "DataTypes/Hand.h"

//Standard
#include <vector>


// Namespaces //


namespace ImageProcessing
{

	class HandDetection
	{
	  public:

		// Detects the hand from a depth image
		static void HandDetect(const DataTypes::DepthImage& depthImage, const DataTypes::Table& table, std::vector<DataTypes::Hand>& hands);

	};
}


#endif