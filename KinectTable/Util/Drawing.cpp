//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Drawing.h"

// Project
#include "DataTypes/Constants.h"

// OpenCV
#include <opencv2\opencv.hpp>

// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------


using namespace DataTypes;

namespace Util
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void Drawing::ConvertToColor(const Types<float>::Image& image, ColorImage& colorImage, bool normalizeValues)
{
	assert(image.rows > 0);
	assert(image.cols > 0);

	// Normalize values
	static Types<float>::Image grayscaleImage;
	if(normalizeValues)
	{
		// Find max
		float maxValue = -99999;
		for(size_t y=0; y<image.rows; y++)
		{
			for(size_t x=0; x<image.cols; x++)
			{
				maxValue = std::max(maxValue, image.data[y][x]);
			}
		}

		grayscaleImage.rows = image.rows;
		grayscaleImage.cols = image.cols;
		for(size_t y=0; y<grayscaleImage.rows; y++)
		{
			for(size_t x=0; x<grayscaleImage.cols; x++)
			{
				grayscaleImage.data[y][x] = image.data[y][x] / maxValue;
			}
		}

	}
	else
	{
		image.CopyTo(grayscaleImage);
	}

	colorImage.rows = grayscaleImage.rows;
	colorImage.cols = grayscaleImage.cols;
	for(size_t y=0; y<colorImage.rows; y++)
	{
		for(size_t x=0; x<colorImage.cols; x++)
		{
			int value = (int)(grayscaleImage.data[y][x] * 255);
			colorImage.data[y][x] = ColorPixel(value, value, value);
		}
	}

	return;
}

void Drawing::ConvertToColor(const BinaryImage& binaryImage, ColorImage& colorImage)
{
	assert(binaryImage.rows > 0);
	assert(binaryImage.cols > 0);

	static const ColorPixel white(255, 255, 255);
	static const ColorPixel black(0, 0, 0);

	colorImage.rows = binaryImage.rows;
	colorImage.cols = binaryImage.cols;
	for(size_t y=0; y<colorImage.rows; y++)
	{
		for(size_t x=0; x<colorImage.cols; x++)
		{
			colorImage.data[y][x] = binaryImage.data[y][x] ? white : black;
		}
	}

	return;
}


void Drawing::ConvertToColor(const DepthImage& image, ColorImage& colorImage, bool normalizeValues)
{
	assert(image.rows > 0);
	assert(image.cols > 0);

	// Normalize values
	if(normalizeValues)
	{
		static Types<float>::Image grayscaleImage;

		// Find max
		DepthPixel maxValue = 0;
		for(size_t y=0; y<image.rows; y++)
		{
			for(size_t x=0; x<image.cols; x++)
			{
				maxValue = std::max(maxValue, image.data[y][x]);
			}
		}

		grayscaleImage.rows = image.rows;
		grayscaleImage.cols = image.cols;
		for(size_t y=0; y<grayscaleImage.rows; y++)
		{
			for(size_t x=0; x<grayscaleImage.cols; x++)
			{
				grayscaleImage.data[y][x] = image.data[y][x] / (float)maxValue;
			}
		}

		Drawing::ConvertToColor(grayscaleImage, colorImage, false);
		return;
	}


	float conversion = 255 / DataTypes::MAX_DEPTH_VALUE;
	colorImage.rows = image.rows;
	colorImage.cols = image.cols;
	for(size_t y=0; y<colorImage.rows; y++)
	{
		for(size_t x=0; x<colorImage.cols; x++)
		{
			int value = (int)(image.data[y][x] * conversion);
			colorImage.data[y][x] = ColorPixel(value, value, value);
		}
	}

	return;
}



void Drawing::DrawPoints(ColorImage& image, const Point2Di& point, const ColorPixel& color, int markerSize)
{
	if(image.rows == 0 || image.cols == 0)
		throw -1;

		
	Point2Di minPoint(point.x - (markerSize - 1), point.y - (markerSize - 1));
	if(minPoint.x < 0)
		minPoint.x = 0;
	if(minPoint.y < 0)
		minPoint.y = 0;

	Point2Di maxPoint(point.x + (markerSize - 1), point.y + (markerSize - 1));
	if(maxPoint.x > (int)image.cols - 1)
		maxPoint.x = image.cols - 1;
	if(maxPoint.y > (int)image.rows - 1)
		maxPoint.y = image.rows - 1;

	for(int y=minPoint.y; y<=maxPoint.y; y++)
	{
		for(int x=minPoint.x; x<=maxPoint.x; x++)
		{
			image.data[y][x] = color;
		}
	}

	return;
}


void Drawing::DrawPoints(ColorImage& image, const std::vector<Point2Di>& points, const ColorPixel& color, int markerSize)
{
	if(image.rows == 0 || image.cols == 0)
		throw -1;

	for(size_t i=0; i<points.size(); i++)
	{
		const Point2Di& point = points[i];
		
		Point2Di minPoint(point.x - (markerSize - 1), point.y - (markerSize - 1));
		if(minPoint.x < 0)
			minPoint.x = 0;
		if(minPoint.y < 0)
			minPoint.y = 0;

		Point2Di maxPoint(point.x + (markerSize - 1), point.y + (markerSize - 1));
		if(maxPoint.x > (int)image.cols - 1)
			maxPoint.x = image.cols - 1;
		if(maxPoint.y > (int)image.rows - 1)
			maxPoint.y = image.rows - 1;

		for(int y=minPoint.y; y<=maxPoint.y; y++)
		{
			for(int x=minPoint.x; x<=maxPoint.x; x++)
			{
				image.data[y][x] = color;
			}
		}
	}

	return;
}

void Drawing::DrawLine(BinaryImage& image, const Point2Di& start, const Point2Di& end, int thickness)
{
	// Get image of line
	const int dataType = CV_8UC1;
	const int lineValue = 1;

	cv::Mat imageMat(image.maxRows, image.maxCols, dataType, (void*)image.data);
	cv::line(imageMat, cv::Point(start.x, start.y), cv::Point(end.x, end.y), lineValue, thickness);

	
	/*
	//Draw the lines between corners
	static Types<float>::Image outputImageF;
	memset(outputImageF.data, 0, sizeof(outputImageF.data));
	cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)outputImageF.data);
	Point2Di last = corners.back();
	Point2Di current = corners.front();
	cv::line(output, cv::Point(current.x, current.y), cv::Point(last.x, last.y), 1);
	*/

	return;
}


void Drawing::DrawLine(ColorImage& image, const Point2Di& start, const Point2Di& end, const ColorPixel& color, int thickness)
{
	// Get image of line
	const int dataType = CV_32FC1;
	const int lineValue = 1;
	static Types<float>::Image lineImage;
	lineImage.rows = lineImage.maxRows;
	lineImage.cols = lineImage.maxCols;
	memset(lineImage.data, 0, sizeof(lineImage.data));
	cv::Mat imageMat(lineImage.maxRows, lineImage.maxCols, dataType, (void*)lineImage.data);
	cv::line(imageMat, cv::Point(start.x, start.y), cv::Point(end.x, end.y), lineValue, thickness);

	// Overlay line on top of color image
	for(size_t y=0; y<lineImage.rows; y++)
	{
		for(size_t x=0; x<lineImage.cols; x++)
		{
			if(lineImage.data[y][x] != 0)
				image.data[y][x] = color;
		}
	}

	/*
	//Draw the lines between corners
	static Types<float>::Image outputImageF;
	memset(outputImageF.data, 0, sizeof(outputImageF.data));
	cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)outputImageF.data);
	Point2Di last = corners.back();
	Point2Di current = corners.front();
	cv::line(output, cv::Point(current.x, current.y), cv::Point(last.x, last.y), 1);
	*/

	return;
}


void Drawing::HighlightRegion(DataTypes::ColorImage& image, const DataTypes::BinaryImage& region, const DataTypes::ColorPixel& color)
{
	for(size_t y=0; y<image.rows; y++)
	{
		for(size_t x=0; x<image.cols; x++)
		{
			image.data[y][x] = region.data[y][x] ? color : image.data[y][x];
		}
	}

	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------





}
