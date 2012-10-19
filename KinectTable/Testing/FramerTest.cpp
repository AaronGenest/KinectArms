//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_NETWORK  //Allow access to module for testing purposes

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "FramerTest.h"

// Project
#include "Network/Framer.h"


// Standard C++


// Standard C
#include <stdio.h>
#include <assert.h>
#include <time.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace Network;


namespace FramerTest
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

Framer framer;


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------


template<typename T>
void CreateRandomImage(typename Types<T>::Image& image);

int TestSubscribeFrame(const Network::Info& info, const KinectDataParams& dataParams);
int TestSubscribeAckFrame(const Network::Info& info, bool accept);
int TestUnsubscribeFrame();
int TestDataParamsFrame(const KinectDataParams& dataParams);
int TestKinectDataFrame(const KinectData& kinectData);
int TestInfoRequestFrame();
int TestInfoFrame(const Network::Info& info);


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


//!! Must add Table and Hands to test!
int RunFramerTest()
{

	int ret = 0;

	printf("Starting Framer test...\n\n");

	// Initialize random seed
	unsigned int seed = (unsigned int)time(NULL);
	Randomization::Randomizer randomizer(seed);
	printf("Seed: %u\n", seed);

	// Prepare Data //

	// Creating info
	printf("Creating info...\n");
	Network::Info info;
	memcpy(info.name, "TestName", sizeof(info.name));
	printf("Info created.\n\n");

	// Creating data params
	printf("Creating Kinect data params...\n");
	KinectDataParams dataParams;
	dataParams.colorImageEnable = true;
	dataParams.depthImageEnable = true;
	dataParams.validityImageEnable = false;
	dataParams.testImageEnable = true;
	dataParams.tableEnable = false;
	dataParams.handsEnable = true;
	printf("Kinect data params created.\n\n");

	

	// Creating data
	printf("Creating Kinect data...\n");
	KinectData kinectData;
	{
		// Params

		kinectData.available.colorImageEnable = true;
		kinectData.available.depthImageEnable = true;
		kinectData.available.validityImageEnable = true;
		kinectData.available.testImageEnable = true;
		kinectData.available.tableEnable = true;
		kinectData.available.handsEnable = true;

		
		// Images

		CreateRandomImage<ColorPixel>(kinectData.colorImage);
		CreateRandomImage<DepthPixel>(kinectData.depthImage);
		CreateRandomImage<BinaryPixel>(kinectData.validityImage);
		CreateRandomImage<ColorPixel>(kinectData.testImage);


		// Table
		randomizer.Randomize(kinectData.table);


		// Creating hands
		const int maxHands = 5;
		kinectData.hands.resize(rand() % maxHands);
		randomizer.RandomizeVector(kinectData.hands);


	}
	printf("Kinect Data created.\n\n");



	// Test Frames //

	// Test Subscribe Frame
	printf("Testing Subscribe Frame...\n");
	ret = TestSubscribeFrame(info, dataParams);
	if(ret != 0)
		return -1;
	printf("Done testing Subscribe Frame.\n\n");

	
	// Test SubscribeAck Frame
	printf("Testing subscribe ack Frame...\n");
	bool acceptSubscription = true;
	ret = TestSubscribeAckFrame(info, acceptSubscription);
	if(ret != 0)
		return -1;
	printf("Done testing SubscribeAck Frame.\n\n");

	// Test Unsubscribe Frame
	printf("Testing Unsubscribe Frame...\n");
	ret = TestUnsubscribeFrame();
	if(ret != 0)
		return -1;
	printf("Done testing Unsubscribe Frame.\n\n");

	// Test DataParams Frame
	printf("Testing DataParams Frame...\n");
	ret = TestDataParamsFrame(dataParams);
	if(ret != 0)
		return -1;
	printf("Done testing DataParams Frame.\n\n");

	
	// Test KinectData Frame
	printf("Testing KinectData Frame...\n");
	ret = TestKinectDataFrame(kinectData);
	if(ret != 0)
		return -1;
	printf("Done testing KinectData Frame.\n\n");


	// Test InfoRequest Frame
	printf("Testing InfoRequest Frame...\n");
	ret = TestInfoRequestFrame();
	if(ret != 0)
		return -1;
	printf("Done testing InfoRequest Frame.\n\n");


	// Test Info Frame
	printf("Testing Info Frame...\n");
	ret = TestInfoFrame(info);
	if(ret != 0)
		return -1;
	printf("Done testing Info Frame.\n\n");
	

	return ret;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------




template<typename T>
void CreateRandomImage(typename Types<T>::Image& image)
{
	typedef char ImageBytes[sizeof(image.data)];

	// Randomize the size of the image
	image.rows = (rand() % image.maxRows);
	image.cols = (rand() % image.maxCols);

	// Randomize the image data
	ImageBytes& imageBytes = (ImageBytes&)image.data;
	for(size_t i=0; i<sizeof(imageBytes); i++)
	{
		imageBytes[i] = (char)(rand() % 0xFF);
	}

	return;
}


int TestSubscribeFrame(const Network::Info& info, const KinectDataParams& dataParams)
{

	int ret;

	// Create the frame
	Framer::Frame frame;
	ret = framer.CreateSubscribe(info, dataParams, frame);
	if(ret != 0)
	{
		printf("Error: Could not create frame.\n");
		return -1;
	}

	// Get frame type
	FrameType::Type frameType;
	ret = framer.GetFrameType(frame, frameType);
	if(ret != 0)
	{
		printf("Error: Could not get frame type.\n");
		return -1;
	}

	// Test the frame type
	if(frameType != FrameType::Subscribe)
	{
		printf("Error: Wrong frame type.\n");
		return -1;
	}

	// Get the frame data
	static Network::Info recvInfo;
	static KinectDataParams recvDataParams;

	ret = framer.RetrieveSubscribe(frame, recvInfo, recvDataParams);
	if(ret != 0)
	{
		printf("Error: Could not get frame data.\n");
		return -1;
	}

	
	// Compare the info
	ret = strcmp(info.name, recvInfo.name);
	if(ret != 0)
	{
		printf("Error: Created and retrieved info are different.\n");
		return -1;
	}

	// Compare the data params
	if(dataParams != recvDataParams)
	{
		printf("Error: Created and retrieved data params are different.\n");
		return -1;
	}

	return 0;
}


int TestSubscribeAckFrame(const Network::Info& info, bool accept)
{
	
	int ret;

	// Create the frame
	Framer::Frame frame;
	ret = framer.CreateSubscribeAck(info, accept, frame);
	if(ret != 0)
	{
		printf("Error: Could not create frame.\n");
		return -1;
	}

	// Get frame type
	FrameType::Type frameType;
	ret = framer.GetFrameType(frame, frameType);
	if(ret != 0)
	{
		printf("Error: Could not get frame type.\n");
		return -1;
	}

	// Test the frame type
	if(frameType != FrameType::SubscribeAck)
	{
		printf("Error: Wrong frame type.\n");
		return -1;
	}

	// Get the frame data
	static Network::Info recvInfo;
	static bool recvAccept;
	ret = framer.RetrieveSubscribeAck(frame, recvInfo, recvAccept);
	if(ret != 0)
	{
		printf("Error: Could not get frame data.\n");
		return -1;
	}

	
	// Compare the info
	ret = strcmp(info.name, recvInfo.name);
	if(ret != 0)
	{
		printf("Error: Created and retrieved info are different.\n");
		return -1;
	}

	// Compare the acceptance
	bool isEqual = accept == recvAccept;
	if(!isEqual)
	{
		printf("Error: Created and retrieved acceptance are different.\n");
		return -1;
	}
	
	
	return 0;
}



int TestUnsubscribeFrame()
{
	
	int ret;

	// Create the frame
	Framer::Frame frame;
	ret = framer.CreateUnsubscribe(frame);
	if(ret != 0)
	{
		printf("Error: Could not create frame.\n");
		return -1;
	}

	// Get frame type
	FrameType::Type frameType;
	ret = framer.GetFrameType(frame, frameType);
	if(ret != 0)
	{
		printf("Error: Could not get frame type.\n");
		return -1;
	}

	// Test the frame type
	if(frameType != FrameType::Unsubscribe)
	{
		printf("Error: Wrong frame type.\n");
		return -1;
	}

	// Get the frame data
	ret = framer.RetrieveUnsubscribe(frame);
	if(ret != 0)
	{
		printf("Error: Could not get frame data.\n");
		return -1;
	}

	return 0;
}


int TestDataParamsFrame(const KinectDataParams& dataParams)
{
	
	int ret;

	// Create the frame
	Framer::Frame frame;
	ret = framer.CreateDataParams(dataParams, frame);
	if(ret != 0)
	{
		printf("Error: Could not create frame.\n");
		return -1;
	}

	// Get frame type
	FrameType::Type frameType;
	ret = framer.GetFrameType(frame, frameType);
	if(ret != 0)
	{
		printf("Error: Could not get frame type.\n");
		return -1;
	}

	// Test the frame type
	if(frameType != FrameType::DataParams)
	{
		printf("Error: Wrong frame type.\n");
		return -1;
	}

	// Get the frame data
	static KinectDataParams recvDataParams;
	ret = framer.RetrieveDataParams(frame, recvDataParams);
	if(ret != 0)
	{
		printf("Error: Could not get frame data.\n");
		return -1;
	}


	// Compare the data params
	if(dataParams != recvDataParams)
	{
		printf("Error: Created and retrieved data params are different.\n");
		return -1;
	}

	return 0;
}



int TestKinectDataFrame(const KinectData& data)
{

	int ret;

	// Create the frame
	Framer::Frame frame;
	ret = framer.CreateKinectData(data.available, data, frame);
	if(ret != 0)
	{
		printf("Error: Could not create frame.\n");
		return -1;
	}

	// Get frame type
	FrameType::Type frameType;
	ret = framer.GetFrameType(frame, frameType);
	if(ret != 0)
	{
		printf("Error: Could not get frame type.\n");
		return -1;
	}

	// Test the frame type
	if(frameType != FrameType::Data)
	{
		printf("Error: Wrong frame type.\n");
		return -1;
	}

	// Get the frame data
	static KinectData recvData;
	ret = framer.RetrieveKinectData(frame, recvData);
	if(ret != 0)
	{
		printf("Error: Could not get frame data.\n");
		return -1;
	}



	// Compare the data params
	if(data.available != recvData.available)
	{
		printf("Error: Created and retrieved data params are different.\n");
		return -1;
	}


	// Compare the Kinect data sizes
	bool isEqual = true;
	
	isEqual &= (data.available.colorImageEnable ? data.colorImage.rows : 0) == recvData.colorImage.rows;
	isEqual &= (data.available.colorImageEnable ? data.colorImage.cols : 0) == recvData.colorImage.cols;
	
	isEqual &= (data.available.depthImageEnable ? data.depthImage.rows : 0) == recvData.depthImage.rows;
	isEqual &= (data.available.depthImageEnable ? data.depthImage.cols : 0) == recvData.depthImage.cols;
	
	isEqual &= (data.available.validityImageEnable ? data.validityImage.rows : 0) == recvData.validityImage.rows;
	isEqual &= (data.available.validityImageEnable ? data.validityImage.cols : 0) == recvData.validityImage.cols;
	
	isEqual &= (data.available.testImageEnable ? data.testImage.rows : 0) == recvData.testImage.rows;
	isEqual &= (data.available.testImageEnable ? data.testImage.cols : 0) == recvData.testImage.cols;

	if(!isEqual)
	{
		printf("Error: A created and retrieved image size is different.\n");
		return -1;
	}


	// Compare the Kinect data values
	isEqual = true;
	if(data.available.colorImageEnable)
	{
		for(size_t y=0; y<data.colorImage.rows; y++)
		{
			for(size_t x=0; x<data.colorImage.cols; x++)
			{
				isEqual &= data.colorImage.data[y][x] == recvData.colorImage.data[y][x];
			}
		}
	}

	
	
	if(data.available.depthImageEnable)
	{
		for(size_t y=0; y<data.depthImage.rows; y++)
		{
			for(size_t x=0; x<data.depthImage.cols; x++)
			{
				isEqual &= data.depthImage.data[y][x] == recvData.depthImage.data[y][x];
			}
		}
	}

	
	if(data.available.validityImageEnable)
	{
		for(size_t y=0; y<data.validityImage.rows; y++)
		{
			for(size_t x=0; x<data.validityImage.cols; x++)
			{
				isEqual &= data.validityImage.data[y][x] == recvData.validityImage.data[y][x];
			}
		}
	}

	if(data.available.testImageEnable)
	{
		for(size_t y=0; y<data.testImage.rows; y++)
		{
			for(size_t x=0; x<data.testImage.cols; x++)
			{
				isEqual &= data.testImage.data[y][x] == recvData.testImage.data[y][x];
			}
		}
	}


	if(!isEqual)
	{
		printf("Error: A created and retrieved image is different.\n");
		return -1;
	}


	// Compare the table
	if(data.available.tableEnable)
	{
		if(data.table != recvData.table)
		{
			printf("Error: Table and retrieved table are different.\n");
			return -1;
		}
	}

	// Compare the hands
	if(data.available.handsEnable)
	{
		if(data.hands != recvData.hands)
		{
			printf("Error: Hands and retrieved hands are different.\n");
			return -1;
		}
	}

	return 0;
}



int TestInfoRequestFrame()
{
	
	int ret;

	// Create the frame
	Framer::Frame frame;
	ret = framer.CreateInfoRequest(frame);
	if(ret != 0)
	{
		printf("Error: Could not create frame.\n");
		return -1;
	}

	// Get frame type
	FrameType::Type frameType;
	ret = framer.GetFrameType(frame, frameType);
	if(ret != 0)
	{
		printf("Error: Could not get frame type.\n");
		return -1;
	}

	// Test the frame type
	if(frameType != FrameType::InfoRequest)
	{
		printf("Error: Wrong frame type.\n");
		return -1;
	}

	// Get the frame data
	ret = framer.RetrieveInfoRequest(frame);
	if(ret != 0)
	{
		printf("Error: Could not get frame data.\n");
		return -1;
	}

	return 0;
}



int TestInfoFrame(const Network::Info& info)
{
	
	int ret;

	// Create the frame
	Framer::Frame frame;
	ret = framer.CreateInfo(info, frame);
	if(ret != 0)
	{
		printf("Error: Could not create frame.\n");
		return -1;
	}

	// Get frame type
	FrameType::Type frameType;
	ret = framer.GetFrameType(frame, frameType);
	if(ret != 0)
	{
		printf("Error: Could not get frame type.\n");
		return -1;
	}

	// Test the frame type
	if(frameType != FrameType::Info)
	{
		printf("Error: Wrong frame type.\n");
		return -1;
	}

	// Get the frame data
	static Network::Info recvInfo;
	ret = framer.RetrieveInfo(frame, recvInfo);
	if(ret != 0)
	{
		printf("Error: Could not get frame data.\n");
		return -1;
	}


	// Compare the info
	ret = strcmp(info.name, recvInfo.name);
	if(ret != 0)
	{
		printf("Error: Created and retrieved info are different.\n");
		return -1;
	}


	return 0;
}



}
