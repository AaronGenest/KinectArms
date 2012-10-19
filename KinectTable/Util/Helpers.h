#ifndef UTIL__HELPERS_H
#define UTIL__HELPERS_H

// Includes //

// Project
//#include "DataTypes/DataTypes.h"

#define MOD_DATA_TYPES //!!
#include "DataTypes/Images.h"
#include "DataTypes/Points.h"


// Standard C++
#include <vector>

// Namespaces //

//!!
namespace Util
{
	class __declspec(dllexport) Helpers
	{

	  public:

		// Gets the system time in milliseconds
		// Millisecond precision in Windows 7.  15ms precision in Windows XP.
		static unsigned long long GetSystemTime();

		// Gets the system directory of the executable currently running
		static int GetExeDirectory(char* filename, int length);

		// Breaks up a color image into its color channels
		static void GetColorChannels(const DataTypes::ColorImage& colorImage, DataTypes::GrayImage& redChannel, DataTypes::GrayImage& greenChannel, DataTypes::GrayImage& blueChannel);

		static void GetBlobFromCorners(const std::vector<DataTypes::Point2Di>& corners, DataTypes::BinaryImage& blob);
		static void GetBlobFromBoundary(const std::vector<DataTypes::Point2Di>& boundary, DataTypes::BinaryImage& blob);
	
		static void GetBlobIndices(const DataTypes::BinaryImage& blob, DataTypes::Types<DataTypes::Point2Di>::FlatImage& indices, bool invertImage = false);

		static void ShowImage(const DataTypes::ColorImage& image);
		static void ShowImage(const DataTypes::DepthImage& image);
		static void ShowImage(const DataTypes::BinaryImage& image);
		static void ShowImage(const DataTypes::Types<float>::Image& image);

	};
}


#endif