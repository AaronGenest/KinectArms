//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SENSORS

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "DataProcessor.h"

// Project
#include "TestImageHolder.h"
#include "ImageProcessing/TableDetection.h"
#include "ImageProcessing/HandDetection.h"
#include "Util/Drawing.h"
#include "Util/Helpers.h"
#include "Util/Snapshot.h"
#include "Util/DemoImageCreator.h"

// OpenCV
#include <opencv2\opencv.hpp>

// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Sensors
{

//---------------------------------------------------------------------------
// Public Static Methods
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

DataProcessor::DataProcessor(const KinectDataParams& params) :
	userParams(params),
	thread(&DataProcessor::ThreadStart),
	frameNumber(0),
	calculateTableCorners(true),
	newDataEvent(CreateEvent(NULL, true, false, "NewDataEvent"))
{
	// Initialize the Kinect
	int ret = KinectSensor::Init();
	if(ret != 0)
		throw ret;

	// Initialize KinectData
	data.available.colorImageEnable = false;
	data.available.depthImageEnable = false;
	data.available.validityImageEnable = false;
	data.available.testImageEnable = false;

	// Start processing thread
	threadExit = false;
	thread.Start(this);

	return;
}


DataProcessor::~DataProcessor()
{	
	// Stop processing thread
	threadExit = true;
	(void)thread.Join();

	// Deinitialize Kinect
	KinectSensor::DeInit();

	CloseHandle(newDataEvent);

	return;
}


void DataProcessor::GetParameters(KinectDataParams& params)
{
	mutex.Lock();
	{
		params = this->userParams;
	}
	mutex.Unlock();

	return;
}

void DataProcessor::SetParameters(const KinectDataParams& params)
{
	mutex.Lock();
	{
		this->userParams = params;
	}
	mutex.Unlock();

	return;
}


// Recalculates the table corners from the next image
void DataProcessor::RecalculateTableCorners()
{
	mutex.Lock();
	{
		calculateTableCorners = true;
	}
	mutex.Unlock();

	return;
}

// NOTE: Can only be called by one thread!
long DataProcessor::GetData(KinectData& data, bool waitForNewData)
{
	long frameNumber;

	if(waitForNewData)
		WaitForSingleObject(newDataEvent, INFINITE);

	mutex.Lock();
	{
		this->data.CopyTo(data);
		frameNumber = this->frameNumber;

		// Specify no data available if we haven't received our first frame yet
		if(frameNumber == 0)
			data.available = KinectDataParams();

		ResetEvent(newDataEvent);
	}
	mutex.Unlock();

	return frameNumber;
}

//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


int DataProcessor::ThreadStart(void* obj)
{
	return ((DataProcessor*)obj)->ProcessingLoop();
}


int DataProcessor::ProcessingLoop()
{
	// Data we are in the moment of processing
	KinectData processingData;
	bool calculateTableCorners;

	// Set up snapshot frame
	char directoryPath[255] = "C:\\Users\\hcilab\\Desktop\\KinectTable Pictures\\"; //"C:\\Users\\Mike\\Desktop\\KinectTable Pictures\\";
	Util::Snapshot::SetDirectoryPath(directoryPath);
	Util::Snapshot::SetSnapshotFrame(100);
	Util::Snapshot::Reset();
	Util::Snapshot::SetEnable(false);

	while(!threadExit)
	{
		// Get data params
		mutex.Lock();
		{
			processingData.available = this->userParams;
			calculateTableCorners = this->calculateTableCorners;
		}
		mutex.Unlock();


		Util::Snapshot::NextFrame();


		// Check if we need any images
		if(processingData.available.colorImageEnable || processingData.available.depthImageEnable || processingData.available.validityImageEnable || processingData.available.testImageEnable || calculateTableCorners || processingData.available.handsEnable)
		{

			// Get raw images
			KinectSensor::GetData(processingData.colorImage, processingData.depthImage, processingData.validityImage);
			
			Util::Snapshot::TakeSnapshot("Raw Color.bmp", processingData.colorImage);
			Util::Snapshot::TakeSnapshot("Raw Depth.bmp", processingData.depthImage);

			//!! This is only here because the Kinect for Windows SDK may have an invalid depth image on the first frame.
			if(processingData.depthImage.rows == 0 && processingData.depthImage.cols == 0)
			{
				continue;
			}

			// Get time of image
			long milliseconds = Util::Helpers::GetSystemTime();
			
			// Check if we need to calculate the table corners
			
			if(processingData.available.depthImageEnable)
			{
				if(calculateTableCorners)
				{
					static BinaryImage tableImage;
					static std::vector<Point2Di> boundary;
					ImageProcessing::TableDetection::TableDetect(processingData.depthImage, tableImage, boundary, table);


					mutex.Lock();
					{
						this->calculateTableCorners = false;
					}
					mutex.Unlock();
				}


				// Calculate hands
				if(processingData.available.handsEnable)
				{
					try
					{
						ImageProcessing::HandDetection::HandDetect(processingData.depthImage, table, processingData.hands);
					}
					catch(...)
					{
						processingData.available.handsEnable = false;
					}

					// Label the hands
					//!!Put this in the HandDetect() function?
					handLabeler.LabelHands(milliseconds, processingData.hands);
				}

			}
		}
				
		// Set table corners in data
		processingData.table = table;


		// Create the test image
		if(processingData.available.testImageEnable)
		{
			//TestImageHolder::GetImageReference().CopyTo(processingData.testImage);
			processingData.available.testImageEnable = Util::DemoImageCreator::CreateGraphicalImage(processingData.testImage, processingData);
			
			Util::Snapshot::TakeSnapshot("Demo Image.bmp", processingData.testImage);
		}


		// Set new data for user
		mutex.Lock();
		{
			frameNumber++;
			processingData.CopyTo(data);
			SetEvent(newDataEvent);
		}
		mutex.Unlock();

	}

	return 0;
}


void DataProcessor::CreateTestImage(KinectData& data)
{

	ColorImage& testImage = data.testImage;

	
	// Use color image as background
	if(data.available.colorImageEnable)
	{
		// Copy over color image
		data.colorImage.CopyTo(testImage);
	}

	/*
	// Create table image
	static BinaryImage tableBlob;
	data.table.CreateTableBlob(tableBlob);
	
	
	// Lighten table
	if(data.available.tableCornersEnable)
	{
		
		// Get indices of points not over the table
		static Types<Point2Di>::FlatImage indices;
		Util::Helpers::GetBlobIndices(tableBlob, indices, true);

		// Darken color image except for over table
		float darkenFactor = 0.25f;
		for(size_t i=0; i<indices.size; i++)
		{
			const Point2Di& point = indices.data[i];
			ColorPixel& colorPixel = testImage.data[point.y][point.x];

			colorPixel.red *= darkenFactor;
			colorPixel.green *= darkenFactor;
			colorPixel.blue *= darkenFactor;
		}
		
	}
	


	
	// Draw arms on image
	if(data.available.handsEnable)
	{
		
		// Draw arm shadows
		for(int i=0; i<data.hands.size(); i++)
		{

			
			// Get arm blob
			static BinaryImage armBlob;
			data.hands[i].CreateArmBlob(armBlob);

			// Get points of arm blob
			static Types<Point2Di>::FlatImage indices;
			Util::Helpers::GetBlobIndices(armBlob, indices);

			// Get shadow shift
			float heightAboveTable = data.table.depth - data.hands[i].meanDepth;
			int xShift = heightAboveTable / 20;
			int yShift = heightAboveTable / 100;

			// Draw shadow
			for(int i=0; i<indices.size; i++)
			{
				const Point2Di& point = indices.data[i];
				
				// Get shifted coordinates
				int xValue = point.x + xShift;
				int yValue = point.y + yShift;

				bool xValueGood = xValue >= 0 && xValue < armBlob.cols;
				bool yValueGood = yValue >= 0 && yValue < armBlob.rows;
				if(xValueGood && yValueGood && !armBlob.data[yValue][xValue])
				{
					ColorPixel& colorPixel = testImage.data[yValue][xValue];
					colorPixel.red = (int)(colorPixel.red * 0.3f);
					colorPixel.green = (int)(colorPixel.green * 0.3f);
					colorPixel.blue = (int)(colorPixel.blue * 0.3f);
				}
			}
			
		}
		
		
		// Draw boundaries around arms
		static const ColorPixel armColors[] = { ColorPixel(ColorPixel::Black), ColorPixel(ColorPixel::Red), ColorPixel(ColorPixel::Green), ColorPixel(ColorPixel::Blue) };
		for(int i=0; i<data.hands.size(); i++)
		{
			const std::vector<Point2Di>& boundary = data.hands[i].boundary;

			if(data.hands[i].id + 1 < sizeof(armColors) / sizeof(ColorPixel));
			{
				for(int j=0; j<boundary.size(); j++)
				{
					const Point2Di& point = boundary[j];
					testImage.data[point.y][point.x] = armColors[data.hands[i].id + 1];
				}
			}
		}
		
		
		// Draw the finger tips
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			Util::Drawing::DrawPoints(testImage, hand.fingerTips, ColorPixel(ColorPixel::Green), 3);
		}

		// Draw the finger bases
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			Util::Drawing::DrawPoints(testImage, hand.fingerBases, ColorPixel(ColorPixel::Red), 3);
		}

		// Draw the arm base
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			if(hand.armBase.x != -1 && hand.armBase.y != -1)
				Util::Drawing::DrawPoints(testImage, hand.armBase, ColorPixel(ColorPixel::Blue), 5);
		}

		
		// Draw the hand palm
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			if(hand.palmCenter.x != -1 && hand.palmCenter.y != -1)
				Util::Drawing::DrawPoints(testImage, hand.palmCenter, ColorPixel(255, 0, 255), 5);
		}
		
	}
	

	//!!
	/*
	if(data.hands.size() > 0)
	{
		static BinaryImage tableEdges;
		table.CreateTableBlob(tableEdges, false);
		
		static BinaryImage armEdges;
		data.hands[0].CreateArmBlob(armEdges, false);


		// Get points in arm base
		for(size_t y=0; y<tableEdges.rows; y++)
		{
			for(size_t x=0; x<tableEdges.cols; x++)
			{
				tableEdges.data[y][x] = tableEdges.data[y][x] && armEdges.data[y][x];
			}
		}
		
		Util::Drawing::ConvertBinaryToColor(armEdges, testImage);

		// Get points in arm base
		for(size_t y=0; y<tableEdges.rows; y++)
		{
			for(size_t x=0; x<tableEdges.cols; x++)
			{
				if(tableEdges.data[y][x])
					testImage.data[y][x] = ColorPixel(ColorPixel::Red);
			}
		}

		if(data.hands[0].armBase.x != -1 && data.hands[0].armBase.y != -1)
			Util::Drawing::DrawPoints(testImage, data.hands[0].armBase, ColorPixel(ColorPixel::Red), 3);

		// Draw the finger tips
		const Hand& hand = data.hands[0];
		Util::Drawing::DrawPoints(testImage, hand.fingerTips, ColorPixel(ColorPixel::Green), 3);
		
	}
	*/
	/*
	if(data.hands.size() > 0)
	{
		static BinaryImage armBlob;
		data.hands[0].CreateArmBlob(armBlob);

		static Types<float>::Image distImage;
		distImage.rows = distImage.maxRows;
		distImage.cols = distImage.maxCols;
		
		cv::Mat input(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)armBlob.data);
		cv::Mat output(DEPTH_RES_Y, DEPTH_RES_X, CV_32FC1, (void*)distImage.data);
		//cv::distanceTransform(input, output, CV_DIST_L2, 3);
		cv::distanceTransform(input, output, CV_DIST_C, 3);

		
		Util::Drawing::ConvertGrayscaleToColor(distImage, testImage, true);

		static Point2Di palmCenter;
		float palmCenterValue = 0;
		for(size_t y=0; y<distImage.rows; y++)
		{
			for(size_t x=0; x<distImage.cols; x++)
			{
				if(distImage.data[y][x] > palmCenterValue)
				{
					palmCenter.x = x;
					palmCenter.y = y;
					palmCenterValue = distImage.data[y][x];
				}
			}
		}

		const float palmThreshold = 0;
		if(palmCenterValue > palmThreshold)
		{
			Util::Drawing::DrawPoints(testImage, palmCenter, ColorPixel(ColorPixel::Red), 3);
		}

	}
	*/

	/*
	// Convert test image to color
	Util::Drawing::ConvertBinaryToColor(tableImage, processingData.testImage);

	// Draw lines of table
	for(size_t i=0; i<processingData.corners.size(); i++)
	{
		const Point2Di& startPoint = processingData.corners[i];
		const Point2Di& endPoint = processingData.corners[(i+1) % processingData.corners.size()];
		Util::Drawing::DrawLine(processingData.testImage, startPoint, endPoint, ColorPixel(255, 0, 0), 3);
	}

	// Draw corners of table
	Util::Drawing::DrawPoints(processingData.testImage, processingData.corners, ColorPixel(0, 0, 255), 3);
	*/

	return;
}




}