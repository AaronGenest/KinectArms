#ifndef IMAGE_PROCESSING__TABLE_DETECTION_H
#define IMAGE_PROCESSING__TABLE_DETECTION_H

// Module Check //
#ifndef MOD_IMAGE_PROCESSING
#error "Files outside module cannot access file directly."
#endif


//
// These functions detect tables in images.
//


// Includes //

//Project
#include "DataTypes/DataTypes.h"

//Standard
#include <vector>


// Namespaces //


namespace ImageProcessing
{

	class TableDetection
	{
	  public:

		// Detects the corners of a table from a depth image
		static void TableDetect(const DataTypes::DepthImage& depthImage, DataTypes::BinaryImage& tableBlob, std::vector<Point2Di>& boundary, DataTypes::Table& table);

	};
}


#endif