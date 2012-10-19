//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "Helpers.h"

// Project
#include "DataTypes/DataTypes.h"

// OpenCV
#include <opencv2\opencv.hpp>

// Standard C++
#include <vector>

// Standard C
#include <Windows.h>
#include <string>
#include <assert.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace std;
using namespace DataTypes;

namespace Util
{

//---------------------------------------------------------------------------
// Data Type Declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

unsigned long long Helpers::GetSystemTime()
{
	// NOTE: This method actually has microsecond precision on Win7 and 0.015s precision on WinXP

	const unsigned long long systemTimeToMilliseconds = 10000;

	SYSTEMTIME systemTime;
	::GetSystemTime(&systemTime);
	FILETIME fileTime;
	bool ret = SystemTimeToFileTime(&systemTime, &fileTime);
	assert(ret);

	ULARGE_INTEGER largeInteger;
	largeInteger.LowPart = fileTime.dwLowDateTime;
	largeInteger.HighPart = fileTime.dwHighDateTime;
	
	unsigned long long retVal = largeInteger.QuadPart / systemTimeToMilliseconds;
	return retVal;
}


int Helpers::GetExeDirectory(char* filename, int length)
{
    GetModuleFileName( NULL, filename, length);
    std::string::size_type pos = std::string(filename).find_last_of( "\\/" );
    filename[pos] = '\0';
	return pos;
}


void Helpers::GetColorChannels(const ColorImage& colorImage, GrayImage& redChannel, GrayImage& greenChannel, GrayImage& blueChannel)
{

	for(size_t y=0; y<colorImage.rows; y++)
	{
		for(size_t x=0; x<colorImage.cols; x++)
		{
			const ColorPixel& pixel = colorImage.data[y][x];
			redChannel.data[y][x] = pixel.red;
			greenChannel.data[y][x] = pixel.green;
			blueChannel.data[y][x] = pixel.blue;
		}
	}

	redChannel.rows = greenChannel.rows = blueChannel.rows = colorImage.rows;
	redChannel.cols = greenChannel.cols = blueChannel.cols = colorImage.cols;

	return;
}


/* //!!
void Helpers::ExportDepthImageToCsv(const char* filename, const DepthImage& depthImage)
{
	
	//Open file
	FILE* file = fopen(filename, "w");
	if(file == NULL)
		throw errno;

	//Write data
	for(size_t y=0; y<depthImage.rows; y++)
	{
		for(size_t x=0; x<depthImage.cols; x++)
		{
			fprintf(file, "%hu,", depthImage.data[y][x]);
		}

		fprintf(file, "\n");
	}


	//Close file
	(void)fclose(file);

	return;
}
*/

void Helpers::GetBlobFromCorners(const std::vector<DataTypes::Point2Di>& corners, DataTypes::BinaryImage& blob)
{
	/*
	std::vector<DataTypes::Point2Di> corners;
	corners.push_back(Point2Di(103, 44));
	corners.push_back(Point2Di(580, 44));
	corners.push_back(Point2Di(580, 360));
	corners.push_back(Point2Di(103, 360));
	*/
	// Clear previous table image
	memset(blob.data, 0, sizeof(blob.data));

	// Convert our binary image to 8 bits
	cv::Mat blobMat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)blob.data);

	// Get table points
	cv::Point* points = new cv::Point[corners.size()];
	for(int i=0; i<corners.size(); i++)
	{
		points[i] = cv::Point(corners[i].x, corners[i].y);
	}

	// Get table image
	const cv::Point* contours[] = { points };
	int pointNum = corners.size();
	cv::fillPoly(blobMat, contours, &pointNum, 1, cv::Scalar(1));

	delete points;
	points = NULL;

	blob.rows = DEPTH_RES_Y;
	blob.cols = DEPTH_RES_X;

	return;
}


void Helpers::GetBlobFromBoundary(const std::vector<Point2Di>& boundary, BinaryImage& blob)
{
	throw ENOTSUP;


	// Clear image data
	memset(blob.data, 0, sizeof(blob.data));

	/*
	IplImage blobImage = cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)blob.data);
	CvScalar color = cv::Scalar(1);
	cvDrawContours(&blobImage, contours[i], color, color, -1, CV_FILLED, 8 );
	*/

	/*
	//!! New way
	// Convert points to openCV points
	std::vector<std::vector<cv::Point>> contours;
	contours.push_back(std::vector<cv::Point>());

	long size = std::min((long)boundary.size(), (long)20);

	contours[0].resize(size);
	for(int i=0; i<size; i++)
		contours[0][i] = cv::Point(boundary[i].x, boundary[i].y);

	printf("Boundary Size = %lu\n", size); fflush(stdout);
	for(int i=0; i<contours[0].size(); i++)
	{
		const cv::Point& point = contours[0][i];
		if(point.x < 0 || point.x >= blob.maxCols || point.y < 0 || point.y >= blob.maxRows)
			assert(false);
	}
	

	cv::Mat blobMat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)blob.data);
	cv::drawContours(blobMat, contours, 0, cv::Scalar(1), CV_FILLED);

	printf("Here\n", boundary.size()); fflush(stdout);
	*/

	// Old way
	{
		/*
		// Create openCV contour
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint)*2, storage);

		for(int i=0; i<boundary.size(); i++)
		{
			CvPoint point = new CvPoint();
			point.x = boundary[i].x;
			point.y = boundary[i].y;
			cvSeqPush(contour, &point);
		}
		*/
		
		

		CvPoint* points = new CvPoint[boundary.size()];
		for(int i=0; i<boundary.size(); i++)
		{
			points[i].x = boundary[i].x;
			points[i].y = boundary[i].y;
		}
		
		
		CvSeq seq_header;
		CvSeq* contour = &seq_header;
		CvSeqBlock block;
		cvMakeSeqHeaderForArray( CV_SEQ_ELTYPE_POINT, sizeof(seq_header), sizeof(CvPoint), points, boundary.size(), &seq_header, &block );
		
		// Draw boundary of arm
		IplImage blobImage = cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)blob.data);
		CvScalar color = cv::Scalar(1);
		cvDrawContours(&blobImage, contour, color, color, CV_FILLED);
			
		delete[] points;
	}

	blob.rows = DEPTH_RES_Y;
	blob.cols = DEPTH_RES_X;
	
	return;
}


// NOTE: Cannot return a std::vector because it slows things down too much when the number of indices are large.
void Helpers::GetBlobIndices(const BinaryImage& blob, Types<Point2Di>::FlatImage& indices, bool invertImage)
{
	indices.size = 0;

	if(!invertImage)
	{
		for(size_t y=0; y<blob.rows; y++)
		{
			for(size_t x=0; x<blob.cols; x++)
			{
				if(blob.data[y][x])
				{
					indices.data[indices.size] = Point2Di(x, y);
					indices.size++;
				}
			}
		}
	}
	else
	{
		for(size_t y=0; y<blob.rows; y++)
		{
			for(size_t x=0; x<blob.cols; x++)
			{
				if(!blob.data[y][x])
				{
					indices.data[indices.size] = Point2Di(x, y);
					indices.size++;
				}
			}
		}
	}

	return;
}

/*
// Create matrices for filtering
// quick opencv structs (does not copy! so ultra fast)
cv::Mat& ConvertFloatImageToMat(const Types<float>::Image& image)
{
	return cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_32FC1, (void*)image.data);
}

cv::Mat& ConvertBinaryImageToMat(const BinaryImage& image)
{
	return cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)arms.data);
}
*/

void Helpers::ShowImage(const ColorImage& image)
{
	assert(image.rows > 0);
	assert(image.cols > 0);

	assert(image.typeSize == 3);
	IplImage* img = cvCreateImage(cv::Size(image.cols, image.rows), IPL_DEPTH_8U, 3);

	int height = img->height;
	int width = img->width;
	int step = img->widthStep/sizeof(uchar);
	int channels = img->nChannels;
	uchar* data = (uchar *)img->imageData;
	for(size_t y=0; y<image.rows; y++)
	{
		for(size_t x=0; x<image.cols; x++)
		{
			data[y*step+x*channels+0] = image.data[y][x].blue;
			data[y*step+x*channels+1] = image.data[y][x].green;
			data[y*step+x*channels+2] = image.data[y][x].red;
		}
	}
	
	// Show image in window
	cvShowImage("Image", img);
	cvWaitKey(0);

	cvReleaseImage(&img);
	return;
}

void Helpers::ShowImage(const DepthImage& image)
{
	assert(image.rows > 0);
	assert(image.cols > 0);

	// Convert depth to floating point values
	static Types<float>::Image imageF;
	imageF.rows = image.rows;
	imageF.cols = image.cols;
	for(size_t y=0; y<imageF.rows; y++)
	{
		for(size_t x=0; x<imageF.cols; x++)
		{
			imageF.data[y][x] = image.data[y][x];
		}
	}

	// Create matrices for filtering
	// quick opencv structs (does not copy! so ultra fast)
	IplImage imageMat = cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_32FC1, (void*)imageF.data);
	cvSetImageROI(&imageMat, cvRect(0, 0, image.cols, image.rows));

	// Show image in window
	cvShowImage("Image", &imageMat);
	cvWaitKey(0);

	cvResetImageROI(&imageMat);
	return;
}


void Helpers::ShowImage(const BinaryImage& image)
{
	assert(image.rows > 0);
	assert(image.cols > 0);

	// Scale pixels to between 0 and 255
	static Types<char>::Image scaledImage;
	scaledImage.rows = image.rows;
	scaledImage.cols = image.cols;
	for(size_t y=0; y<scaledImage.rows; y++)
	{
		for(size_t x=0; x<scaledImage.cols; x++)
		{
			scaledImage.data[y][x] = image.data[y][x] ? 255 : 0;
		}
	}

	// Create matrices for filtering
	// quick opencv structs (does not copy! so ultra fast)
	IplImage imageMat = cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)scaledImage.data);
	cvSetImageROI(&imageMat, cvRect(0, 0, image.cols, image.rows));

	// Show image in window
	cvShowImage("Image", &imageMat);
	cvWaitKey(0);

	cvResetImageROI(&imageMat);
	return;
}

void Helpers::ShowImage(const Types<float>::Image& image)
{
	assert(image.rows > 0);
	assert(image.cols > 0);

	// Create matrices for filtering
	// quick opencv structs (does not copy! so ultra fast)
	IplImage imageMat = cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_32FC1, (void*)image.data);
	cvSetImageROI(&imageMat, cvRect(0, 0, image.cols, image.rows));

	// Show image in window
	cvShowImage("Image", &imageMat);
	cvWaitKey(0);

	//cv::imShow("Image", mat);
	//cv::waitKey(0);

	cvResetImageROI(&imageMat);
	return;
}



/*
int GetLocalIpAddress(carray<char,255> ipAddress)
{
	int ret;

	// Initialize winsock
	WSAData wsaData;
	WSAStartup(MAKEWORD(1, 1), &wsaData);


	// Get host name
	char hostName[80];
	ret = gethostname(hostName, sizeof(hostName));
	if(ret != 0)
		return -1;

	// Get host information
	struct hostent* host = gethostbyname(hostName);
	if(host == NULL)
		return -1;

	// Get first ip address
	
}
*/

}