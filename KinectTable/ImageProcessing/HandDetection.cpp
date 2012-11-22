//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_IMAGE_PROCESSING

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "HandDetection.h"

// OpenCV
#include <opencv2\opencv.hpp>

// Project
#include "Helpers.h"
#include "DataTypes/HandPimpl.h"
#include "Util/Drawing.h"
#include "Util/Helpers.h"
#include "Util/Snapshot.h"
#include "Sensors/TestImageHolder.h"
#include "DataTypes/Pixels.h"

// cvblobslib
//#include "BlobResult.h"

// Standard
#include <exception>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace std;
using namespace cv;
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

void GetHandBoundaries(const BinaryImage& armBlobs, std::vector<Hand>& hands);
void FindArmBase(const std::vector<Point2Di>& boundary, const BinaryImage& tableEdges, Point2Di& armBase);
void FindFingerTips(const std::vector<Point2Di>& boundary, const Point2Di& armBase, const Point2Di& armCenter, std::vector<Point2Di>& fingerTips, std::vector<Point2Di>& fingerBases);
void FindPalmCenter(const BinaryImage& armBlob, const Point2Di& armBase, const Point2Di& armCenter, Point2Di& palmCenter);

//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------



// Detects the corners of the table
void HandDetection::HandDetect(const DepthImage& depthImage, const Table& table, std::vector<Hand>& hands)
{

	
	if(table.corners.size() == 0)
		throw -1;


	// Clear any previous hands
	hands.clear();

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
	//!! Do we really want to do this?
	/*
	for(size_t y=0; y<depthImageF.rows; y++)
	{
		for(size_t x=0; x<depthImageF.cols; x++)
		{
			depthImageF.data[y][x] /= depthMax;
		}
	}
	*/ 

	
	
	// Get table
	static BinaryImage tableBlob;
	table.CreateTableBlob(tableBlob);
	
	

	// Remove everything but above table
	static BinaryImage armBlobs;
	armBlobs.rows = depthImageF.rows;
	armBlobs.cols = depthImageF.cols;
	{
		// Get depth of table
		const float tableDepth = table.depth;

		// Remove everything around table and table and invalid pixels
		for(size_t y=0; y<armBlobs.rows; y++)
		{
			for(size_t x=0; x<armBlobs.cols; x++)
			{
				armBlobs.data[y][x] = (tableBlob.data[y][x]) && (depthImageF.data[y][x] < tableDepth) && (depthImageF.data[y][x] != 0);
			}
		}

		// Put a black border around image so we still get boundaries of arms even when the table takes up the whole image
		for(size_t y=0; y<armBlobs.rows; y++)
		{
			armBlobs.data[y][0] = 0;
			armBlobs.data[y][armBlobs.cols - 1] = 0;
		}
		for(size_t x=0; x<armBlobs.cols; x++)
		{
			armBlobs.data[0][x] = 0;
			armBlobs.data[armBlobs.rows - 1][x] = 0;
		}

	}

	
	Util::Snapshot::TakeSnapshot("Arm 1 - Blobs Above Table.bmp", armBlobs);
	
	// Get edges around arms
	static BinaryImage armEdges;
	armEdges.rows = armEdges.maxRows;
	armEdges.cols = armEdges.maxCols;
	{

		
		static DataTypes::Types<char>::Image depth8U;
		depth8U.rows = depth8U.maxRows;
		depth8U.cols = depth8U.maxCols;
		for(size_t y=0; y<depth8U.rows; y++)
		{
			for(size_t x=0; x<depth8U.cols; x++)
			{
				depth8U.data[y][x] = depthImageF.data[y][x] * 255 / (float)DataTypes::MAX_DEPTH_VALUE;
			}
		}
		

		static BinaryImage cannyEdges;
		cannyEdges.rows = cannyEdges.maxRows;
		cannyEdges.cols = cannyEdges.maxCols;
		{
			
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)depth8U.data);
			static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)cannyEdges.data);

			cv::Canny(input, output, 12, 7, 3);
		}

		Util::Snapshot::TakeSnapshot("Arm 2 - Canny Edge Detection.bmp", cannyEdges);

		
		static BinaryImage erodedArmBlobs;
		erodedArmBlobs.rows = erodedArmBlobs.maxRows;
		erodedArmBlobs.cols = erodedArmBlobs.maxCols;
		{
			
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)armBlobs.data);
			static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)erodedArmBlobs.data);

			int closingNum = 1;
			cv::erode(input, output, cv::Mat(), cv::Point(-1,-1), closingNum);
		}

		
		static BinaryImage armDivider;
		armDivider.rows = armDivider.maxRows;
		armDivider.cols = armDivider.maxCols;
		for(size_t y=0; y<depth8U.rows; y++)
		{
			for(size_t x=0; x<depth8U.cols; x++)
			{
				armDivider.data[y][x] = (cannyEdges.data[y][x] && erodedArmBlobs.data[y][x]) || depthImage.data[y][x] == 0;
				//armDivider.data[y][x] = cannyEdges.data[y][x] && erodedArmBlobs.data[y][x];
			}
		}

		/*static ColorImage test;
		test.rows = 640;
		test.cols = 640;
		Sensors::TestImageHolder::SetImage(armBlobs);*/
		
		/*static DataTypes::ColorImage testImage;
		Util::Drawing::ConvertToColor(armDivider, testImage);
		Sensors::TestImageHolder::SetImage(testImage);*/
	

		static BinaryImage dilatedArmDivider;
		dilatedArmDivider.rows = dilatedArmDivider.maxRows;
		dilatedArmDivider.cols = dilatedArmDivider.maxCols;
		{
			
			static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)armDivider.data);
			static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)dilatedArmDivider.data);

			int closingNum = 2;
			cv::dilate(input, output, cv::Mat(), cv::Point(-1,-1), closingNum);
		}

		
		
		for(size_t y=0; y<depth8U.rows; y++)
		{
			for(size_t x=0; x<depth8U.cols; x++)
			{
				armEdges.data[y][x] = cannyEdges.data[y][x] || dilatedArmDivider.data[y][x];
			}
		}

		
	}
	
	Util::Snapshot::TakeSnapshot("Arm 3 - Arm Edges.bmp", armEdges);


	

	// Take intersection of edges and armblobs
	for(int y=0; y<DEPTH_RES_Y; y++)
	{
		for(int x=0; x<DEPTH_RES_X; x++)
		{
			armBlobs.data[y][x] = armBlobs.data[y][x] && !armEdges.data[y][x];
		}
	}


	Util::Snapshot::TakeSnapshot("Arm 4 - Separate Arm Blobs.bmp", armBlobs);


	/*
	static DataTypes::ColorImage testImage;
	Util::Drawing::ConvertToColor(armBlobs, testImage);
	Sensors::TestImageHolder::SetImage(testImage);
	*/

	
	// Get hand boundaries
	GetHandBoundaries(armBlobs, hands);


	
	
	// Get table edges
	static BinaryImage tableEdges;
	table.CreateTableBlob(tableEdges, false);


	
	// Dilate table edges so that blobs are intersecting table
	static BinaryImage dilatedTableEdges;
	dilatedTableEdges.rows = dilatedTableEdges.maxRows;
	dilatedTableEdges.cols = dilatedTableEdges.maxCols;
	{
		static cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, CV_8U, (void*)tableEdges.data);
		static cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, CV_8U, (void*)dilatedTableEdges.data);

		const int closingNum = 2;
		cv::dilate(input, output, cv::Mat(), cv::Point(-1,-1), closingNum);
		
	}
	
	

	// Find base of each arm
	for(size_t i=0; i<hands.size(); i++)
		FindArmBase(hands[i].boundary, dilatedTableEdges, hands[i].armBase);

	// Calculate the center of geometry
	for(size_t i=0; i<hands.size(); i++)
	{
		// Find center of geometry of the arm
		Point2Di center;
		for(size_t j=0; j<hands[i].boundary.size(); j++)
			center += hands[i].boundary[j];

		center.x = (int)(center.x / hands[i].boundary.size());
		center.y = (int)(center.y / hands[i].boundary.size());

		hands[i].centroid = center;
	}

	// Calculate mean depth and area and palm center
	for(size_t i=0; i<hands.size(); i++)
	{
		// Get arm blob
		static BinaryImage armBlob;
		hands[i].CreateArmBlob(armBlob);

		// Get arm blob indices
		static Types<Point2Di>::FlatImage armBlobIndices;
		Util::Helpers::GetBlobIndices(armBlob, armBlobIndices);

		// Calculate mean depth
		float depth = 0;
		for(size_t j=0; j<armBlobIndices.size; j++)
		{
			Point2Di point = armBlobIndices.data[j];
			depth += depthImageF.data[point.y][point.x];
		}
		depth /= armBlobIndices.size;

		hands[i].meanDepth = depth;
		hands[i].area = armBlobIndices.size;


		// Find palm center
		FindPalmCenter(armBlob, hands[i].armBase, hands[i].centroid, hands[i].palmCenter);
	}

	// Find finger tips and points between fingers
	for(size_t i=0; i<hands.size(); i++)
	{
		FindFingerTips(hands[i].boundary, hands[i].armBase, hands[i].centroid, hands[i].fingerTips, hands[i].fingerBases);
	}
	

	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------

void GetHandBoundaries(const BinaryImage& armBlobs, std::vector<Hand>& hands)
{
		
	cv::Mat armBlobsMat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)armBlobs.data);
		
		
	// Get the contours of the blobs
	static CvSeq* firstContour = 0;
	CvMat armsBlobCvMat = armBlobsMat;
	static CvMemStorage* storage = cvCreateMemStorage(0);
	int numContours = cvFindContours(&armsBlobCvMat, storage, &firstContour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		
		
	// Put contours in a nice list and get rid of all small contours
	const int smallBoundarySize = 40;
	std::vector<CvSeq*> contours;
	for(CvSeq* contour = firstContour; contour!= 0; contour = contour->h_next)
	{			
		// Remove any small boundaries
		if(contour->total < smallBoundarySize)
			continue;

		contours.push_back(contour);
	}


	// Clear all previous hands
	hands.clear();

		
	// Get the hand boundaries (take as many as we are allowed)
	hands.resize(contours.size());
	for(size_t i=0; i<hands.size(); i++)
	{
		CvSeq& contour = *contours[i];
			
			
		((HandPimpl*)hands[i].handPimpl)->SetBoundary(contour);

		CvSeqReader reader;
		cvStartReadSeq(&contour, &reader, 0);

		for(int j=0; j<contour.total; j++)
		{
			CvPoint point;
			memcpy(&point, reader.ptr, contour.elem_size);

			hands[i].boundary.push_back(Point2Di(point.x, point.y));

			CV_NEXT_SEQ_ELEM(contour.elem_size, reader);
		}

			
	}

		
	return;
}



// Looks at the boundary of the arm that touches the edge of the table.
// Takes the largest continous boundary that touches the table because sometimes a little piece of the arm might touch in another spot.
//!!Assumes boundary points are in order!
void FindArmBase(const std::vector<Point2Di>& boundary, const BinaryImage& tableEdges, Point2Di& armBase)
{
	

	// Find a boundary point that isn't on the table edge
	int startIndex = -1;
	for(size_t j=0; j<boundary.size(); j++)
	{
		if(!tableEdges.data[boundary[j].y][boundary[j].x])
		{
			startIndex = j;
			break;
		}
	}
			
	if(startIndex == -1)
	{
		armBase = Point2Di(-1, -1);
		return;
	}
	

	// Make variables to hold properties of the arm base
	int maxCount = 0;
	int maxStartIndex = 0;

	// Go around boundary looking for the largest segment
	{
		// Find base start
		int baseStartIndex = 0;
		int count = 0;
		for(size_t j=0; j<boundary.size(); j++)
		{
			const Point2Di& point = boundary[(j + startIndex) % boundary.size()];
			const Point2Di& fwrdPoint = boundary[(j + startIndex + 1) % boundary.size()];
			
			bool startingBase = !tableEdges.data[point.y][point.x] && tableEdges.data[fwrdPoint.y][fwrdPoint.x];
			bool endingBase = tableEdges.data[point.y][point.x] && !tableEdges.data[fwrdPoint.y][fwrdPoint.x];

			if(tableEdges.data[point.y][point.x])
				count++;

			if(startingBase)
			{
				// We are starting a base
				baseStartIndex = (j + startIndex + 1) % boundary.size();
				count = 0;
			}
			else if(endingBase)
			{
				if(count > maxCount)
				{
					maxCount = count;
					maxStartIndex = baseStartIndex;
				}
			}

		}

		// Check if we found any base
		if(maxCount == 0)
		{
			armBase = Point2Di(-1, -1);
			return;
		}
	
	}

	// Calculate the center of the base
	int armBaseIndex = ((maxStartIndex + (maxStartIndex + maxCount)) / 2) % boundary.size();
	armBase = boundary[armBaseIndex];

	return;
}



//!! Could just ignore all points that are between the geometric center and the arm base
void FindFingerTips(const std::vector<Point2Di>& boundary, const Point2Di& armBase, const Point2Di& armCenter, std::vector<Point2Di>& fingerTips, std::vector<Point2Di>& fingerBases)
{

	// Get curvature of boundary
	const int k = 30;  //!!Should use a linear function of boundary length to figure out k
	vector<float> curvature;
	KCurvature(boundary, k, curvature);

	// Flip curvature
	for(size_t j=0; j<curvature.size(); j++)
	{
		curvature[j] = (float)PI - curvature[j];
	}

	// Find peaks of curvature
	//!! Could use non-maximal suppression to find local extrema!
	float threshold = 1.5; //70 * (float)PI / 180;
	vector<int> peakIndices;
	FindLocalMaxima(curvature, (float)PI - threshold, peakIndices);


	// Separate peaks into finger tips and finger bases
	std::vector<int> fingerTipIndices;
	std::vector<int> fingerBaseIndices;
	{
		int boundaryLength = boundary.size();
		for(size_t peakNum=0; peakNum<peakIndices.size(); peakNum++)
		{
			int j = peakIndices[peakNum];

			int backIndex = (j - k) % boundaryLength;
			if(backIndex < 0)
				backIndex += boundaryLength;

			int fwrdIndex = (j + k) % boundaryLength;
			if(fwrdIndex >= boundaryLength)
				fwrdIndex = boundaryLength - fwrdIndex;

			float backX = (float)(boundary[j].x - boundary[backIndex].x);
			float backY = (float)(boundary[j].y - boundary[backIndex].y);
			float fwrdX = (float)(boundary[j].x - boundary[fwrdIndex].x);
			float fwrdY = (float)(boundary[j].y - boundary[fwrdIndex].y);
			Point3Df backVector(backX, backY, 0);
			Point3Df fwrdVector(fwrdX, fwrdY, 0);
    
			bool isFingerTip = Point3Df::Cross(backVector, fwrdVector).z > 0;
			if(isFingerTip)
				fingerTipIndices.push_back(j);
			else
				fingerBaseIndices.push_back(j);

					
		}
	}


	// Only add points that are farther from the arm base than the geometric center
	float armCenterMagnitude = (armCenter - armBase).Magnitude();
	for(size_t j=0; j<fingerTipIndices.size(); j++)
	{
		const Point2Di& point = boundary[fingerTipIndices[j]];
		const float pointToArmBaseDist = (point - armBase).Magnitude();
		const float pointToArmCenterDist = (point - armCenter).Magnitude();
		
		if(pointToArmBaseDist > pointToArmCenterDist) {
			fingerTips.push_back(point);
		}
	}
	
	for(size_t j=0; j<fingerBaseIndices.size(); j++)
	{
		const Point2Di& point = boundary[fingerBaseIndices[j]];
		float pointMagnitude = (point - armBase).Magnitude();
		
		if(pointMagnitude > armCenterMagnitude)
			fingerBases.push_back(point);
	}

	// Convert finger tips to points on image
	// Ignore any points on the table edge
	/*
	for(size_t j=0; j<fingerTipIndices.size(); j++)
	{
		const int index = fingerTipIndices[j];
		const Point2Di& point = boundary[index];
		const Point2Di& backPoint = boundary[(index - 1) % boundary.size()];
		const Point2Di& fwrdPoint = boundary[(index + 1) % boundary.size()];

		if(!tableEdges.data[point.y][point.x] && !tableEdges.data[backPoint.y][backPoint.x] && !tableEdges.data[fwrdPoint.y][fwrdPoint.x])
			fingerTips.push_back(point);
	}

	// Convert finger bases to points on image
	// Ignore any points on the table edge
	for(size_t j=0; j<fingerBaseIndices.size(); j++)
	{
		const int index = fingerBaseIndices[j];
		const Point2Di& point = boundary[index];
		const Point2Di& backPoint = boundary[(index - 1) % boundary.size()];
		const Point2Di& fwrdPoint = boundary[(index + 1) % boundary.size()];

		if(!tableEdges.data[point.y][point.x] && !tableEdges.data[backPoint.y][backPoint.x] && !tableEdges.data[fwrdPoint.y][fwrdPoint.x])
			fingerBases.push_back(point);
	}
	*/

	return;
}


void FindPalmCenter(const BinaryImage& armBlob, const Point2Di& armBase, const Point2Di& armCenter, Point2Di& palmCenter)
{

	static Types<float>::Image distImage;
	distImage.rows = distImage.maxRows;
	distImage.cols = distImage.maxCols;
		
	cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)armBlob.data);
	cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, CV_32FC1, (void*)distImage.data);
	cv::distanceTransform(input, output, CV_DIST_C, 3);  //!!Could use CV_DIST_L2 for euclidian distance

	// Only check points that are farther from the arm base than the geometric center
	float armCenterMagnitude = (armCenter - armBase).Magnitude();
	
	// Get arm blob indices
	static Types<Point2Di>::FlatImage armBlobIndices;
	Util::Helpers::GetBlobIndices(armBlob, armBlobIndices, false);

	// Find the max value of the distance transform that is further from arm base than geometric center is
	static Point2Di maxValuePoint;
	float maxValue = 0;
	for(size_t i=0; i<armBlobIndices.size; i++)
	{
		const Point2Di& point = armBlobIndices.data[i];
		float pointMagnitude = (point - armBase).Magnitude();

		const float value = distImage.data[point.y][point.x] * pointMagnitude;
		if(pointMagnitude > armCenterMagnitude && value > maxValue)
		{
			maxValuePoint = point;
			maxValue = value;
		}

	}
	
	const float palmThreshold = 0;
	if(maxValue > palmThreshold)
		palmCenter = maxValuePoint;
	else
		palmCenter = Point2Di(-1, -1);

	return;
}

}
