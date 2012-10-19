
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "FramerSpeedTest.h"

// Project
#include "Framer.h"
#include "Sockets.h"


// Standard C++


// Standard C
#include <stdio.h>
#include <assert.h>
#include <time.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------


using namespace Network;

namespace FramerSpeedTest
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

// Size of images to send (must be less than or equal to IMAGE_ROWS and IMAGE_COLS)
const unsigned long imageRows = IMAGE_ROWS;
const unsigned long imageCols = IMAGE_COLS;


const char address[] = "127.0.0.1";
const unsigned short portNum = 55555;


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------

int RunServer();
int RunClient();

template<typename T>
void CreateRandomImage(typename Types<T>::Image& image, size_t rows, size_t cols);


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


int RunFramerSpeedTest()
{

	int ret;

	printf("Starting Framer speed test...\n\n");

	// Assert some pre-test conditions
	assert(imageRows <= IMAGE_ROWS);
	assert(imageCols <= IMAGE_COLS);

	
	// Print out menu
	printf("Is this the client or server?\n");
	printf("Server = 0\n");
	printf("Client = 1\n");

	// Get input from user
	char input;
	scanf("%c%*c", &input);


	switch(input)
	{
		case '0': ret = RunServer(); break;
		case '1': ret = RunClient(); break;
		default: assert(false); break;
	}
	
	
	return ret;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


int RunServer()
{

	int ret;

	printf("Creating socket listener...\n");
	printf("\tAddress: %s\n", address);
	printf("\tPort Num: %hu\n", portNum);
	Sockets::TcpListener* listenerPtr;
	try
	{
		listenerPtr = new Sockets::TcpListener(address, portNum);
	}
	catch (...)
	{
		printf("Error: Could not create socket listener.\n");
		return -1;
	}
	Sockets::TcpListener& listener = *listenerPtr;
	printf("Created socket listener.\n\n");

	
	// Listen
	printf("Starting to listen for sockets...\n");
	ret = listener.StartListening();
	if(ret != 0)
	{
		printf("Error: Could not listen.\n");
		return -1;
	}
	printf("Listening for sockets.\n\n");


	// Accept
	printf("Waiting for a socket...\n");
	Sockets::TcpSocket* socketPtr = listener.Accept();
	if(socketPtr == NULL)
	{
		printf("Error: Could not accept a socket.\n");
		return -1;
	}
	Sockets::TcpSocket& socket = *socketPtr;
	printf("Accepted a socket.\n\n");


	
	// Receive/Send data
	printf("Receiving and sending back data...");
	KinectData kinectData;
	KinectDataParams dataParams;
	while(true)
	{

		// Receive the next message
		MessageType::Type messageType;
		ret = Framer::ReceiveMessage(socket, messageType);
		if(ret != 0)
		{
			printf("Error: Could not receive message.\n");
			return -1;
		}

		// Make sure we have the proper message
		if(messageType != MessageType::Data)
		{
			printf("Error: Received the wrong message type.\n");
			return -1;
		}

		// Get the message data
		ret = Framer::RecvKinectData(dataParams, kinectData);
		if(ret != 0)
		{
			printf("Error: Could not get message data.\n");
			return -1;
		}

		// Send data
		ret = Framer::SendKinectData(socket, dataParams, kinectData);
		if(ret != 0)
		{
			printf("Error: Could not send data.\n");
			return -1;
		}
		

		//Show user we are still doing something
		printf(".");
	}

	return 0;
}


int RunClient()
{

	// Connect
	printf("Connecting socket...\n");
	printf("\tAddress: %s\n", address);
	printf("\tPort Num: %hu\n", portNum);

	Sockets::TcpSocket* socketPtr = Sockets::TcpSocket::Connect(address, portNum);
	if(socketPtr == NULL)
	{
		printf("Error: Could not connect to server.\n");
		return -1;
	}
	Sockets::TcpSocket& socket = *socketPtr;
	printf("Socket is connected.\n\n");


	// Initialize random seed
	srand((unsigned int)time(NULL));

	// Creating data params
	KinectDataParams dataParams;
	dataParams.colorImageEnable = true;
	dataParams.depthImageEnable = true;
	dataParams.validityImageEnable = true;
	dataParams.tableImageEnable = true;

	// Creating data
	printf("Creating Kinect data...\n");
	KinectData kinectData;
	CreateRandomImage<ColorPixel>(kinectData.colorImage, imageRows, imageCols);
	CreateRandomImage<DepthPixel>(kinectData.depthImage, imageRows, imageCols);
	CreateRandomImage<BinaryPixel>(kinectData.validityImage, imageRows, imageCols);
	CreateRandomImage<BinaryPixel>(kinectData.tableImage, imageRows, imageCols);

	size_t kinectDataBytes = (sizeof(ColorPixel) + sizeof(DepthPixel) + 2 * sizeof(BinaryPixel)) * imageRows * imageCols;
	printf("Kinect Data created.\n\n");


	// Send/Receive data
	printf("Sending/Receiving data...\n");
	size_t frameNum = 0;
	size_t seconds = 0;
	unsigned long firstMilliseconds = GetTickCount();
	while(true)
	{
		int ret;
		unsigned long startMilliseconds = GetTickCount();

		// Send data
		ret = Framer::SendKinectData(socket, dataParams, kinectData);
		if(ret != 0)
		{
			printf("Error: Could not send data.\n");
			return -1;
		}


		// Receive the next message
		MessageType::Type messageType;
		ret = Framer::ReceiveMessage(socket, messageType);
		if(ret != 0)
		{
			printf("Error: Could not receive message.\n");
			return -1;
		}

		// Make sure we have the proper message
		if(messageType != MessageType::Data)
		{
			printf("Error: Received the wrong message type.\n");
			return -1;
		}

		// Get the message data
		ret = Framer::RecvKinectData(dataParams, kinectData);
		if(ret != 0)
		{
			printf("Error: Could not get message data.\n");
			return -1;
		}

		

		frameNum++;
		unsigned long endMilliseconds = GetTickCount();

		// Print out frames per second
		unsigned long elapsedSeconds = (endMilliseconds - firstMilliseconds) / 1000;
		
		if(elapsedSeconds > seconds)
		{

			double fps = 1 / ((double)(endMilliseconds - startMilliseconds) / 1000);
			double aveFps = frameNum / ((double)(endMilliseconds - firstMilliseconds) / 1000);
			const size_t bitsPerFrame = kinectDataBytes * 8;
			printf("Data sent and received.\n");
			printf("\tAve FPS: %g   -   Now FPS: %g\n", aveFps, fps);
			printf("\tAve kbps: %g   -   Now kbps: %g\n", aveFps * bitsPerFrame / (double)1000, fps * bitsPerFrame / (double)1000);
			
			seconds = elapsedSeconds;
		}
	
	}

	return 0;
}


template<typename T>
void CreateRandomImage(typename Types<T>::Image& image, size_t rows, size_t cols)
{
	typedef char ImageBytes[sizeof(image.data)];

	// Randomize the size of the image
	image.rows = rows;
	image.cols = cols;

	// Randomize the image data
	ImageBytes& imageBytes = (ImageBytes&)image.data;
	for(size_t i=0; i<sizeof(imageBytes); i++)
	{
		imageBytes[i] = (char)(rand() % 0xFF);
	}

	return;
}


}
