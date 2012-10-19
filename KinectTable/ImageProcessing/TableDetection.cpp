//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_IMAGE_PROCESSING

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "ImageProcessing/TableDetection.h"

// Project
#include "Helpers.h"
#include "DataTypes/DataTypes.h"
#include "Util/Snapshot.h"


// OpenCV
#include <opencv2\opencv.hpp>
//#include <opencv2\imgproc\imgproc.hpp>

// Standard
#include <exception>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace std;

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



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------



// Detects the corners of the table
void TableDetection::TableDetect(const DepthImage& depthImage, BinaryImage& tableBlob, std::vector<Point2Di>& boundary, DataTypes::Table& table)
{
	// Clear previous table image
	memset(tableBlob.data, 0, sizeof(tableBlob.data));

	// Clear previous boundary
	boundary.clear();

	// Clear table data
	table.corners.clear();
	table.depth = 0;

	//!!If image is smaller than max size, then should copy everything to a smaller array so there isn't as much filtering?
	assert(depthImage.rows == DEPTH_RES_Y);
	assert(depthImage.cols == DEPTH_RES_X);


	// Get max of image
	//!!Should we just use constant MAX_DEPTH_VALUE?
	long depthMax = 0;
	for(size_t y=0; y<depthImage.rows; y++)
	{
		for(size_t x=0; x<depthImage.cols; x++)
		{
			if(depthImage.data[y][x] > depthMax)
				depthMax = depthImage.data[y][x];
		}
	}

	// Convert depth to floating point values
	static Types<float>::Image depthImageF;
	depthImageF.rows = depthImage.rows;
	depthImageF.cols = depthImage.cols;
	for(size_t y=0; y<depthImageF.rows; y++)
	{
		for(size_t x=0; x<depthImageF.cols; x++)
		{
			depthImageF.data[y][x] = depthImage.data[y][x];
		}
	}

	// Scale image by max value
	for(size_t y=0; y<depthImageF.rows; y++)
	{
		for(size_t x=0; x<depthImageF.cols; x++)
		{
			depthImageF.data[y][x] /= depthMax;
		}
	}
	
	
	// Get table blob
	//!!BinaryImage table;
	tableBlob.rows = DEPTH_RES_Y;
	tableBlob.cols = DEPTH_RES_X;
	{


		// Get edges of image using the laplacian filter
		static Types<float>::Image edges;
		edges.rows = edges.maxRows;
		edges.cols = edges.maxCols;
		{
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)depthImageF.data);
			static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)edges.data);

			CvMat in = input;
			CvMat out = output;
			cvLaplace(&in, &out, 1);  //cv::Laplacian(smoothed, laplacian, 1);
		}

		Util::Snapshot::TakeSnapshotNow("Table 1 - Laplacian.bmp", edges);


		// Threshold the edges
		static Types<float>::Image threshedEdges;
		threshedEdges.rows = threshedEdges.maxRows;
		threshedEdges.cols = threshedEdges.maxCols;
		{
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)edges.data);
			static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)threshedEdges.data);

			const float threshold = 0.05f;
			(void)cv::threshold(input, output, threshold, 1, CV_THRESH_BINARY); //cv::adaptiveThreshold(laplacian, thresh, 1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 
		}
		
		Util::Snapshot::TakeSnapshotNow("Table 2 - Thresholded Laplacian.bmp", threshedEdges);
		

		// Close the edges
		static Types<float>::Image closedEdges;
		closedEdges.rows = closedEdges.maxRows;
		closedEdges.cols = closedEdges.maxCols;
		{
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)threshedEdges.data);
			static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)closedEdges.data);

			//!! Might be faster if we are using BinaryImage instead of a float image
			const int closingNum = 1;
			
			//static cv::Mat dilated(DEPTH_RES_Y, DEPTH_RES_X, dataType);
			cv::dilate(input, output, cv::Mat(), cv::Point(-1,-1), closingNum);
			//cv::erode(dilated, closedEdges, cv::Mat(), cv::Point(-1,-1), closingNum);
		}
		
		Util::Snapshot::TakeSnapshotNow("Table 3 - Closed Edges.bmp", closedEdges);


		// Choose the center blob as the table
		Point2Di tablePixelPos(DEPTH_RES_Y / 2, DEPTH_RES_X / 2);
		{
			// Start at center of image and scan to the right until we find a pixel value that isn't invalid (depth of zero) and is not an edge.
			while((depthImageF.data[tablePixelPos.y][tablePixelPos.x] == INVALID_DEPTH_VALUE) || (closedEdges.data[tablePixelPos.y][tablePixelPos.x]))
			{
				tablePixelPos.x++;

				// Just return if we do not find a suitable pixel to start our fill
				assert(tablePixelPos.y >= 0);
				assert(tablePixelPos.x >= 0);
				if((unsigned int)tablePixelPos.y >= depthImageF.rows || (unsigned int)tablePixelPos.x >= depthImageF.cols)
					return;
			}
		}


		// Flood the table region
		{
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)closedEdges.data);

			const int tableLabel = 2;
			cv::floodFill(input, cv::Point(DEPTH_RES_X/2, DEPTH_RES_Y/2), tableLabel);
		}
		
		Util::Snapshot::TakeSnapshotNow("Table 4 - Flooded Table Region.bmp", closedEdges);


		// Keep only the table region
		static Types<float>::Image tableRegion;
		tableRegion.rows = tableRegion.maxRows;
		tableRegion.cols = tableRegion.maxCols;
		{
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)closedEdges.data);
			static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)tableRegion.data);

			//NOTE: We only count the table up to the inside of the edge because it is the smoother than the outside of the edge.
			(void)cv::threshold(input, output, 1.5, 1, CV_THRESH_BINARY);
		}
		

		// Convert our binary image to 8 bits
		{
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)tableRegion.data);
			static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)tableBlob.data);
			input.convertTo(output, CV_8UC1);
		}

		Util::Snapshot::TakeSnapshotNow("Table 5 - Cleaned Table Region.bmp", tableBlob);


		// Put a black border around image so we still get boundary of table even when the table takes up the whole image
		for(size_t y=0; y<tableBlob.rows; y++)
		{
			tableBlob.data[y][0] = 0;
			tableBlob.data[y][tableBlob.cols - 1] = 0;
		}
		for(size_t x=0; x<tableBlob.cols; x++)
		{
			tableBlob.data[0][x] = 0;
			tableBlob.data[tableBlob.rows - 1][x] = 0;
		}
		
	}

	
	// Get table boundary
	//!!vector<Point2Di> boundary;
	{
		
		
		// Get the boundaries
		static CvSeq* firstContour;
		CvMat tableBlobCvMat = cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)tableBlob.data);
		static CvMemStorage* storage = cvCreateMemStorage(0);
		cvFindContours(&tableBlobCvMat, storage, &firstContour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		//static vector<vector<cv::Point>> contours;
		//cv::findContours(tableBlob, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		//vector<cv::Point>& tableContour = contours[0];
		
		// Select the largest boundary
		CvSeqReader reader;
		CvSeq* largestContour = NULL;
		int largestContourPoints = 0;
		for(CvSeq* contour = firstContour; contour != NULL; contour = contour->h_next)
		{
			cvStartReadSeq(contour, &reader, 0);
			if(contour->total > largestContourPoints)
			{
				largestContour = contour;
				largestContourPoints = contour->total;
			}
		}
		
		// We're done if there is no table found
		//!! Should also return if the boundary is not long enough
		if(largestContour == NULL)
			return;

		// Get the boundary points
		cvStartReadSeq(largestContour, &reader, 0);
		for(int i=0; i<largestContour->total; i++)
		{
			CvPoint point;
			memcpy(&point, reader.ptr, largestContour->elem_size);

			boundary.push_back(Point2Di(point.x, point.y));

			CV_NEXT_SEQ_ELEM(largestContour->elem_size, reader);
		}

	}
	

	
	// Find corners of boundary
	{

		// Get curvature of boundary
		const int k = 30;  //!!Should use a linear function of boundary length to figure out k
		vector<float> curvature;
		KCurvature(boundary, k, curvature);

		// Flip curvature
		for(size_t i=0; i<curvature.size(); i++)
		{
			curvature[i] = (float)PI - curvature[i];
		}

		// Find peaks of curvature
		//!! Could use non-maximal suppression to find local extrema!
		float threshold = 70 * (float)PI / 180;
		vector<int> peakIndices;
		FindLocalMaxima(curvature, threshold, peakIndices);

		// Convert curvature peaks to points on image
		for(size_t i=0; i<peakIndices.size(); i++)
		{
			//Point2Di& point = boundary[peakIndices[i]];
			//table.corners.push_back(ImagePoint(point, depthImage.data[point.y][point.x]));
			table.corners.push_back(boundary[peakIndices[i]]);
		}

	}
	
	
	// Calculate a good value for table height
	{
		//!! Could make an image of estimated depths of the table for each pixel
		//!! Could just hold onto the image we used to calculate the table corners
		DepthPixel minValue = MAX_DEPTH_VALUE;
		float sum = 0;
		for(size_t i=0; i<table.corners.size(); i++)
		{
			DepthPixel cornerDepth = depthImage.data[table.corners[i].y][table.corners[i].x];
			sum += cornerDepth;
			minValue = std::min(minValue, cornerDepth);
		}
		const DepthPixel tableDepth = minValue; //sum / tableCorners.size(); //0.7f;
		const DepthPixel validHeightAboveTable = tableDepth - 30;  //!! Look at noise variance of table to choose this?

		table.depth = validHeightAboveTable;
	}

	return;
}





/*
void TableDetectHoughTransform(const DepthImage& inputImage, BinaryImage& outputImage)
{

	static Types<float>::Image inputImageF;
	static Types<float>::Image outputImageF;

	for(int y=0; y<inputImage.rows; y++)
	{
		for(int x=0; x<inputImage.cols; x++)
		{
			inputImageF.data[y][x] = inputImage.data[y][x] / (float)MAX_DEPTH_VALUE;
		}
	}

	 
	// Create filtering matrices
	// quick opencv structs (does not copy! so ultra fast)
	//!!Dangerous code! don't know internals of cv::Mat.
	cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)inputImageF.data);
	cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)outputImageF.data);
	

	//Smooth the image  //!!Shouldn't need to do this if KinectSensor does preprocessing
	static cv::Mat smoothed(DEPTH_RES_Y, DEPTH_RES_X, dataType);
	cv::blur(input, smoothed, cv::Size(5,5));
	
	//Get the laplacian (second-derivative)
	static cv::Mat laplacian(DEPTH_RES_Y, DEPTH_RES_X, dataType);
	CvMat in = smoothed;
	CvMat out = laplacian;
	cvLaplace(&in, &out, 1);  //cv::Laplacian(smoothed, laplacian, 1);
	
	//Scale the laplacian
	laplacian *= 100;

	//Threshold the laplacian
	static cv::Mat thresh(DEPTH_RES_Y, DEPTH_RES_X, dataType);
	(void)cv::threshold(laplacian, thresh, 0.5, 1, CV_THRESH_BINARY); //cv::adaptiveThreshold(laplacian, thresh, 1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 

	//Make the lines thicker
	//static cv::Mat dilated(DEPTH_RES_Y, DEPTH_RES_X, dataType);
	cv::dilate(thresh, output, cv::Mat());

	//Fill and label all the regions
	/*
	int x;
	int y;
	int regionNum = 2;
	while(FindBackgroundPixel(outputImageF, x, y))
	{
		cv::floodFill(output, cv::Point(x,y), regionNum);
		regionNum++;
	}
	*/
	
	//Get the hough lines
	/*
	unsigned short blah[DEPTH_RES_Y][DEPTH_RES_X];
	for(int y=0; y<DEPTH_RES_Y; y++)
	{
		for(int x=0; x<DEPTH_RES_X; x++)
		{
			float value = outputImageF[y][x];
			if(value > 1)
				blah[y][x] = 255;
			else if(value < 0)
				blah[y][x] = 0;
			else
				blah[y][x] = value * 255;
		}
	}
	cv::Mat houghInput(DEPTH_RES_Y, DEPTH_RES_X, CV_8U, (void*)blah);
	
	std::vector<cv::Vec2f> lines;
	cv::HoughLines(houghInput, lines, 1, CV_PI/180, 100);
	/*cv::Mat colorOutput(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC3, (void*)outputImage);
	for(int i=0; i < std::min((int)lines.size(),100); i++ )
    {
        cv::Vec2f& line = lines[i];
        float rho = line[0];
        float theta = line[1];
        cv::Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        cv::line(colorOutput, pt1, pt2, CV_RGB(255,0,0), 3, 8);
    }
	*/
	/*
	CvMat hough = output;
	CvMat colorOutput = cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, dataType, (void*)outputImage);
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	lines = cvHoughLines2(&hough, lines, CV_HOUGH_STANDARD, 1, CV_PI/180, 100, 0, 0);
	for(int i = 0; i < MIN(lines->total,100); i++ )
    {
        float* line = (float*)cvGetSeqElem(lines,i);
        float rho = line[0];
        float theta = line[1];
        CvPoint pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        cvLine(&hough, pt1, pt2, CV_RGB(255,0,0), 3, 8 );
    }
	*//*

	
	//Convert the image to binary
	for(int y=0; y<DEPTH_RES_Y; y++)
	{
		for(int x=0; x<DEPTH_RES_X; x++)
		{
			outputImage[y][x] = outputImageF[y][x] == 1 ? true : false;
			//outputImage[y][x] = outputImageF[y][x] == (regionNum -1) ? true : false;
		}
	}
	
	return;
}
*/


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


//Gets the corners of the contour.
//The contour points must be sorted in CW or CCW order.
//void GetCorners(vector<Point2Di>& contour, vector<Point2Di>& corners)
//{
	// Constants //
	/*
	const float maxThreshold = 0.5f;
	const float minThreshold = 0.2f;
	
	const int windowSize = 40; //20;
	const int averageFilterWindowSize = 10;

	//Get angle of difference of points when going around boundary
	vector<float> angles;
	int pointNum = contour.size();
	assert(windowSize < pointNum);
	for(int i=0; i<pointNum; i++)
	{

		//Get point before current point
		int backIndex = i-windowSize/2;
		if(backIndex < 0)
			backIndex = pointNum + backIndex;
		Point2Di& back = contour[backIndex];

		//Get point ahead of current point
		int frontIndex = i+windowSize/2;
		if(frontIndex >= pointNum)
			frontIndex = frontIndex - pointNum;
		Point2Di& front = contour[frontIndex];

		int rise = front.y - back.y;
		int run = front.x - back.x;
		angles.push_back(atan2((float)rise, (float)run));

	}

	
	//Smooth out the data
	vector<float> smoothedAngles;
	AverageFilter(angles, averageFilterWindowSize, smoothedAngles);

	
	//Get the change of angles when going around boundary
	vector<float> slopes;
	pointNum = smoothedAngles.size();
	for(int i=0; i<pointNum; i++)
	{
		//Get current value
		float current = smoothedAngles[i];

		//Get value before current value
		int backIndex = i-1;
		if(backIndex < 0)
			backIndex = pointNum + backIndex;
		float back = smoothedAngles[backIndex];

		//Get smallest change between slopes
		float diff = abs(current - back);
		diff = min<float>(diff, (float)(2*PI - diff));

		slopes.push_back(diff);
	}


	//Normalize the slopes
	float maxSlope = *max_element(slopes.begin(), slopes.end());
	pointNum = slopes.size();
	for(int i=0; i<pointNum; i++)
	{
		slopes[i] /= maxSlope;
		slopes[i] *= slopes[i];
	}


	
	//Find max value of each peak that is above threshold//

	//Find the min point to start at so we don't start on a peak
	int startIndex = distance(slopes.begin(), min_element(slopes.begin(), slopes.end()));

	//Move across slopes, grabbing the max value at each peak
	vector<Point1Df> peaksMax;
	bool inPeak = false;
	Point1Df currentPeakMax;
	currentPeakMax.value = 0;
	for(int i=0; i<pointNum; i++)
	{
		//Get current value
		int index = i+startIndex;
		if(index >= pointNum)
			index = index - pointNum;
		float current = slopes[index];

		//Check if we are in a peak
		bool oldInPeak = inPeak;
		inPeak = current >= maxThreshold;

		//Get max value if we are in a peak
		if(inPeak)
		{
			if(current > currentPeakMax.value)
			{
				currentPeakMax.value = current;
				currentPeakMax.index = index;
			}
		}

		//Check if we have just left a peak
		if(oldInPeak && !inPeak)
		{
			//Save the max of the peak we just left
			peaksMax.push_back(currentPeakMax);

			//Reset the current max value for the next peak
			currentPeakMax.value = 0;
		}
	}

	
	//The max points of each peak are our corners.
	//Get the cartesian coordinates of the points.
	for(size_t i=0; i<peaksMax.size(); i++)
	{
		corners.push_back(contour[peaksMax[i].index]);
	}
	
	*/
//	return;
//}





}
