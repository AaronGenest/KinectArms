//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_IMAGE_PROCESSING

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Helpers.h"

// Project
#include "DataTypes/Images.h"
#include "DataTypes/Points.h"

// OpenCV
#include <opencv2\opencv.hpp>
//#include <opencv2\imgproc\imgproc.hpp>

// Standard
#include <exception>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace std;
using namespace DataTypes;

namespace ImageProcessing
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

const ColorPixel blackPixel(0, 0, 0);
const int dataType = CV_32FC1;
const double PI = 3.141592;

typedef struct tagPoint1Df
{
	int index;
	float value;
} Point1Df;

//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------


bool FindBackgroundPixel(const float (&image)[DEPTH_RES_Y][DEPTH_RES_X], int& x, int& y);
void AverageFilter(const vector<float>& input, int windowSize, vector<float>& output);


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


void KCurvature(const std::vector<Point2Di>& boundary, int k, std::vector<float>& curvature)
{

	// Clear any previous curvature data
	curvature.resize(boundary.size());

	int boundaryLength = boundary.size();
	for(int i=0; i<boundaryLength; i++)
	{
		int backIndex = (i - k) % boundaryLength;
		if(backIndex < 0)
			backIndex += boundaryLength;

		int fwrdIndex = (i + k) % boundaryLength;
		if(fwrdIndex >= boundaryLength)
			fwrdIndex = boundaryLength - fwrdIndex;

		Point2Di backVector(boundary[i].x - boundary[backIndex].x, boundary[i].y - boundary[backIndex].y);
		Point2Di fwrdVector(boundary[i].x - boundary[fwrdIndex].x, boundary[i].y - boundary[fwrdIndex].y);
    
		curvature[i] = std::acos(Point2Di::Dot(backVector, fwrdVector) / (backVector.Magnitude() * fwrdVector.Magnitude()));
	}

	return;
}


void FindLocalMaxima(const std::vector<float>& values, float threshold, std::vector<int>& peakIndices)
{
	const float floatMin = -99999; //!! The lowest number we can have

	// Clear any previous peaks
	peakIndices.clear();

	bool lastIsPeak = false;
	float maxValue = floatMin;
	int currentPeakIndex = -1;
	for(size_t i=0; i<values.size(); i++)
	{
		float value = values[i];
		bool isPeak = value > threshold;

		if(!isPeak && lastIsPeak)
		{
			assert(currentPeakIndex != -1);
			peakIndices.push_back(currentPeakIndex);
			maxValue = floatMin;
			currentPeakIndex = -1;
		}

		if(value > maxValue)
		{
			maxValue = value;
			currentPeakIndex = i;
		}

		lastIsPeak = isPeak;
	}
    

	return;
}



void AverageFilter(const vector<float>& input, int windowSize, vector<float>& output)
{
	assert((size_t)windowSize < input.size());

	int pointNum = input.size();
	for(int i=0; i<pointNum; i++)
	{
		//Get current value
		float current = input[i];

		//Get average for the current point
		float numerator = 0;
		int windowStart = i-windowSize/2;
		int windowEnd = i+windowSize/2;
		for(int j = windowStart; j <= windowEnd; j++)
		{
			//Find the index after wrapping around the data
			int index = j;
			if(index < 0)
				index += pointNum;
			else if(index >= pointNum)
				index -= pointNum;

			//Get the value at the index
			float value = input[index];

			//Get the value relative to the current value
			float theta = value - current;
			
			//Wrap the value around the circle
			if(theta > (float)PI)
				theta -= 2*(float)PI;
			else if(theta < -PI)
				theta += 2*(float)PI;

			//Add the value to the numerator
			numerator += theta;
		}

		//Get average of values in window and add the offset back in
		output.push_back(numerator / windowSize + current);

	}

	return;
}



bool FindBackgroundPixel(const float (&image)[DEPTH_RES_Y][DEPTH_RES_X], int& x, int& y)
{
	for(y=0; y<DEPTH_RES_Y; y++)
	{
		for(x=0; x<DEPTH_RES_X; x++)
		{
			if(image[y][x] == 0)
				return true;
		}
	}

	return false;
}




void DepthToColor(const DepthImage& depthImage, ColorImage& colorImage)
{
	cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)depthImage.data);
	cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)colorImage.data);
	cv::cvtColor(input, output, CV_GRAY2BGR );
	return;
}

void DepthThreshold(const DepthImage& input, DepthPixel minValue, DepthPixel maxValue, const ColorImage& color, ColorImage& output)
{
	
	for(size_t y=0; y<input.rows; y++)
	{
		for(size_t x=0; x<input.cols; x++)
		{
			DepthPixel value = input.data[y][x];
			output.data[y][x] = (value >= minValue && value <= maxValue) ? color.data[y][x] : blackPixel;
		}
	}

	return;
}



void DepthThreshold(const DepthImage& input, DepthPixel minValue, DepthPixel maxValue, DepthImage& output)
{
	for(size_t y=0; y<input.rows; y++)
	{
		for(size_t x=0; x<input.cols; x++)
		{
			DepthPixel value = input.data[y][x];
			output.data[y][x] = (value >= minValue && value <= maxValue) ? value : 0;
		}
	}

	output.rows = input.rows;
	output.cols = input.cols;

	return;
}



void Skeletonize(const DataTypes::BinaryImage& inputImage, DataTypes::BinaryImage& outputImage, int iterations)
{
	
	
	static DataTypes::BinaryImage tempImage;
	tempImage.rows = tempImage.maxRows;
	tempImage.cols = tempImage.maxCols;

	static DataTypes::BinaryImage erodedImage;
	erodedImage.rows = erodedImage.maxRows;
	erodedImage.cols = erodedImage.maxCols;

	static DataTypes::BinaryImage skelImage;
	skelImage.rows = skelImage.maxRows;
	skelImage.cols = skelImage.maxCols;

	inputImage.CopyTo(outputImage);
	
	for(int y=0; y<skelImage.rows; y++)
	{
		for(int x=0; x<skelImage.cols; x++)
		{
			skelImage.data[y][x] = false;
		}
	}
	
	
	//cv::Mat skel(img.size(), CV_8UC1, cv::Scalar(0));
	cv::Mat img(DEPTH_RES_Y, DEPTH_RES_X, CV_8U, (void*)outputImage.data);
	cv::Mat skel(DEPTH_RES_Y, DEPTH_RES_X, CV_8U, (void*)skelImage.data);
	cv::Mat temp(DEPTH_RES_Y, DEPTH_RES_X, CV_8U, (void*)tempImage.data);
	cv::Mat eroded(DEPTH_RES_Y, DEPTH_RES_X, CV_8U, (void*)erodedImage.data);
 
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
 
	bool done;		
	do
	{
	  cv::erode(img, eroded, element);
	  cv::dilate(eroded, temp, element); // temp = open(img)
	  cv::subtract(img, temp, temp);
	  cv::bitwise_or(skel, temp, skel);
	  eroded.copyTo(img);
 
	  done = (cv::norm(img) == 0);
	} while (!done);


	skelImage.CopyTo(outputImage);
	return;

}


}
