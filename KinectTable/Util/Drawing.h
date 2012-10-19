#ifndef UTIL__DRAWING_H
#define UTIL__DRAWING_H


//
// This holds a bunch of functions to help drawing on an image.
//


// Includes //


// Project
//#include "DataTypes/DataTypes.h"

#define MOD_DATA_TYPES //!!
#include "DataTypes/Images.h"
#include "DataTypes/Points.h"


// Standard C++
#include <vector>

// Standard C




// Namespaces //




namespace Util
{
	
	class __declspec(dllexport) Drawing
	{

	  public:

		// PUBLIC //

		// Converts a float image to a grayscale image
		static void ConvertToColor(const DataTypes::Types<float>::Image& image, DataTypes::ColorImage& colorImage, bool normalizeValues = false);

		// Converts a binary image into a black and white image
		static void ConvertToColor(const DataTypes::BinaryImage& binaryImage, DataTypes::ColorImage& colorImage);

		// Converts a depth image into a grayscale image
		static void ConvertToColor(const DataTypes::DepthImage& depthImage, DataTypes::ColorImage& colorImage, bool normalizeValues = false);

		// Draws points on the image
		static void DrawPoints(DataTypes::ColorImage& image, const DataTypes::Point2Di& point, const DataTypes::ColorPixel& color, int markerSize = 1);
		static void DrawPoints(DataTypes::ColorImage& image, const std::vector<DataTypes::Point2Di>& points, const DataTypes::ColorPixel& color, int markerSize = 1);

		// Draws a line between two points
		static void DrawLine(DataTypes::BinaryImage& image, const DataTypes::Point2Di& start, const DataTypes::Point2Di& end, int thickness);
		static void DrawLine(DataTypes::ColorImage& image, const DataTypes::Point2Di& start, const DataTypes::Point2Di& end, const DataTypes::ColorPixel& color, int thickness = 1);

		// Highlights the corresponding region
		static void HighlightRegion(DataTypes::ColorImage& image, const DataTypes::BinaryImage& region, const DataTypes::ColorPixel& color);
	};

}


#endif