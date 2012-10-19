#ifndef IMAGE_PROCESSING__HELPERS_H
#define IMAGE_PROCESSING__HELPERS_H


// Module Check //
#ifndef MOD_IMAGE_PROCESSING
#error "Files outside module cannot access file directly."
#endif



//
// These functions manipulate images.
//


// Includes //

//Project
#include "DataTypes/Images.h"
#include "DataTypes/Points.h"

//Standard
#include <vector>


// Namespaces //


namespace ImageProcessing
{


	//Will keep all color values that have a corresponding depth pixel within the min/max range inclusively.
	//Input and output color arrays can be the same memory.
	void DepthThreshold(const DataTypes::DepthImage& input, DataTypes::DepthPixel minValue, DataTypes::DepthPixel maxValue, const DataTypes::ColorImage& color, DataTypes::ColorImage& output);


	//Will keep all values within the min/max range inclusively.  Will set all others to zero.
	//Input and output arrays can be the same memory.
	void DepthThreshold(const DataTypes::DepthImage& input, DataTypes::DepthPixel minValue, DataTypes::DepthPixel maxValue, DataTypes::DepthImage& output);

	// Gets the curvature of each point on the boundary.  Assumes the boundary is a closed curved (ie. connected at both ends).
	void KCurvature(const std::vector<DataTypes::Point2Di>& boundary, int k, std::vector<float>& curvature);
	
	// Finds the local maxima above the specified threshold.  Returns the indices of the peaks.
	void FindLocalMaxima(const std::vector<float>& values, float threshold, std::vector<int>& peakIndices);

	void Skeletonize(const DataTypes::BinaryImage& input, DataTypes::BinaryImage& output, int iterations);

	template<typename T>
	static void Crop(typename const DataTypes::Types<T>::Image& input, const DataTypes::Point2Di& min, const DataTypes::Point2Di& max, typename DataTypes::Types<T>::Image& output);

}

// Template files
#include "Helpers.hpp"


#endif