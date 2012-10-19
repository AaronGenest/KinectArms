//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_NETWORK  //Allow access to module for testing purposes

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "ClientTest.h"

// Project
#include "Sockets/Sockets.h"
#include "Network/Client.h"
#include "Network/RemoteClient.h"


// Standard C++


// Standard C
#include <stdio.h>
#include <assert.h>
#include <time.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace Network;

/*
namespace ClientTest
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------


const char address[] = "127.0.0.1";
const unsigned short portNum = 55555;


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------

int RunServer();
int RunClient();

template<typename T>
void CreateRandomImage(typename Types<T>::Image& image);

int TestSubscribeMessage(Sockets::TcpSocket& socket, const Network::Info& info, const KinectDataParams& dataParams);
int TestSubscribeAckMessage(Sockets::TcpSocket& socket, const Network::Info& info, bool accept);
int TestUnsubscribeMessage(Sockets::TcpSocket& socket);
int TestDataParamsMessage(Sockets::TcpSocket& socket, const KinectDataParams& dataParams);
int TestKinectDataMessage(Sockets::TcpSocket& socket, const KinectDataParams& dataParams, const KinectData& kinectData);
int TestInfoRequestMessage(Sockets::TcpSocket& socket);
int TestInfoMessage(Sockets::TcpSocket& socket, const Network::Info& info);

Info clientInfo;
Info serverInfo;

KinectDataParams dataParams;


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

//!! Test does not handle changing the info or data params.
int RunClientTest()
{

	int ret;

	printf("Starting Framer test...\n\n");

	/*
	// Creating data params
	KinectDataParams dataParams;
	dataParams.colorImageEnable = true;
	dataParams.depthImageEnable = true;
	dataParams.validityImageEnable = false;
	dataParams.testImageEnable = true;

	// Creating data
	printf("Creating Kinect data...\n");
	KinectData kinectData;
	CreateRandomImage<ColorPixel>(kinectData.colorImage);
	CreateRandomImage<DepthPixel>(kinectData.depthImage);
	CreateRandomImage<BinaryPixel>(kinectData.validityImage);
	CreateRandomImage<BinaryPixel>(kinectData.testImage);
	printf("Kinect Data created.\n\n");
	*//*
	
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
	
	
	


	// Create remote client
	RemoteClient client(socket);

	// Wait for client subscription
	printf("Waiting for client subscription...\n");
	while(client.GetSubscriptionState() == SubscriptionState::Pending);
	printf("Received client subscription.\n\n");

	// Get client info
	printf("Getting client info...\n");
	Info recvInfo;
	client.GetInfo(recvInfo);
	printf("Received client info.\n\n");

	// Make sure we got the proper info
	printf("Comparing client info...\n");
	ret = strcmp(clientInfo.name, recvInfo.name);
	if(ret != 0)
	{
		printf("Received info did not match client info.\n");
		return -1;
	}
	printf("Received info matches client info.\n\n");


	// Accept client subscription
	printf("Accepting client subscription...\n");
	ret = client.AcceptSubscription(serverInfo);
	if(ret != 0)
	{
		printf("Could not accept subscription.\n");
		return -1;
	}
	printf("Accepted client subscription.\n\n");

	// Get client data params
	printf("Getting client data params.\n");
	KinectDataParams recvDataParams;
	client.GetKinectDataParams(recvDataParams);
	printf("Received client data params.\n\n");


	// Make sure we got the proper data params
	printf("Comparing client data params...\n");
	bool isEqual = true;
	isEqual &= dataParams.colorImageEnable == recvDataParams.colorImageEnable;
	isEqual &= dataParams.depthImageEnable == recvDataParams.depthImageEnable;
	isEqual &= dataParams.validityImageEnable == recvDataParams.validityImageEnable;
	isEqual &= dataParams.testImageEnable == recvDataParams.testImageEnable;
	
	if(!isEqual)
	{
		printf("Received data params did not match client data params.\n");
		return -1;
	}
	printf("Received data params matches client data params.\n\n");

	// Send Kinect data
	printf("Sending Kinect data...\n");
	ret = client.SetKinectData(data, dataParams);
	if(ret != 0)
	{
		printf("Could not send Kinect data.\n");
		return -1;
	}
	printf("Sent Kinect data.\n\n");

	// Wait for client to unsubscribe
	while(client.GetSubscriptionState() == SubscriptionState::Subscribed);


	return 0;
}




int RunClient()
{
	int ret;


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
	

	// Creating info
	Network::Info info;
	memcpy(info.Name(), "TestName", sizeof(info.Name()));

	// Creating data params
	KinectDataParams dataParams;
	dataParams.colorImageEnable = true;
	dataParams.depthImageEnable = true;
	dataParams.validityImageEnable = false;
	dataParams.testImageEnable = true;

	// Creating data
	printf("Creating Kinect data...\n");
	KinectData kinectData;
	CreateRandomImage<ColorPixel>(kinectData.colorImage);
	CreateRandomImage<DepthPixel>(kinectData.depthImage);
	CreateRandomImage<BinaryPixel>(kinectData.validityImage);
	CreateRandomImage<BinaryPixel>(kinectData.testImage);
	printf("Kinect Data created.\n\n");


	// Test Subscribe message
	printf("Testing Subscribe message...\n");
	ret = TestSubscribeMessage(socket, info, dataParams);
	if(ret != 0)
		return -1;
	printf("Done testing Subscribe message.\n\n");

	
	// Test SubscribeAck message
	printf("Testing subscribe ack message...\n");
	bool acceptSubscription = true;
	ret = TestSubscribeAckMessage(socket, info, acceptSubscription);
	if(ret != 0)
		return -1;
	printf("Done testing SubscribeAck message.\n\n");

	// Test Unsubscribe message
	printf("Testing Unsubscribe message...\n");
	ret = TestUnsubscribeMessage(socket);
	if(ret != 0)
		return -1;
	printf("Done testing Unsubscribe message.\n\n");

	// Test DataParams message
	printf("Testing DataParams message...\n");
	ret = TestDataParamsMessage(socket, dataParams);
	if(ret != 0)
		return -1;
	printf("Done testing DataParams message.\n\n");

	
	// Test KinectData message
	printf("Testing KinectData message...\n");
	ret = TestKinectDataMessage(socket, dataParams, kinectData);
	if(ret != 0)
		return -1;
	printf("Done testing KinectData message.\n\n");


	// Test InfoRequest message
	printf("Testing InfoRequest message...\n");
	ret = TestInfoRequestMessage(socket);
	if(ret != 0)
		return -1;
	printf("Done testing InfoRequest message.\n\n");


	// Test Info message
	printf("Testing Info message...\n");
	ret = TestInfoMessage(socket, info);
	if(ret != 0)
		return -1;
	printf("Done testing Info message.\n\n");


	return 0;
}


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


int TestSubscribeMessage(Sockets::TcpSocket& socket, const Network::Info& info, const KinectDataParams& dataParams)
{

	int ret;

	// Create data to use for comparison
	static Network::Info recvInfo;
	static KinectDataParams recvDataParams;

	// Send the message
	ret = Framer::SendSubscribe(socket, info, dataParams);
	if(ret != 0)
	{
		printf("Error: Could not send Subscribe message.\n");
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
	if(messageType != MessageType::Subscribe)
	{
		printf("Error: Received the wrong message type.\n");
		return -1;
	}

	// Get the message data
	ret = Framer::RecvSubscribe(recvInfo, recvDataParams);
	if(ret != 0)
	{
		printf("Error: Could not get message data.\n");
		return -1;
	}

	// Compare the info
	ret = strcmp(info.Name(), recvInfo.Name());
	if(ret != 0)
	{
		printf("Error: Sent and received info are different.\n");
		return -1;
	}

	// Compare the data params
	bool isEqual = true;
	isEqual &= dataParams.colorImageEnable == recvDataParams.colorImageEnable;
	isEqual &= dataParams.depthImageEnable == recvDataParams.depthImageEnable;
	isEqual &= dataParams.validityImageEnable == recvDataParams.validityImageEnable;
	isEqual &= dataParams.testImageEnable == recvDataParams.testImageEnable;
	
	if(!isEqual)
	{
		printf("Error: Sent and received data params are different.\n");
		return -1;
	}

	return 0;
}


int TestSubscribeAckMessage(Sockets::TcpSocket& socket, const Network::Info& info, bool accept)
{

	int ret;

	// Create data to use for comparison
	static Network::Info recvInfo;
	static bool recvAccept;

	// Send the message
	ret = Framer::SendSubscribeAck(socket, info, accept);
	if(ret != 0)
	{
		printf("Error: Could not send SubscribeAck message.\n");
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
	if(messageType != MessageType::SubscribeAck)
	{
		printf("Error: Received the wrong message type.\n");
		return -1;
	}

	// Get the message data
	ret = Framer::RecvSubscribeAck(recvInfo, recvAccept);
	if(ret != 0)
	{
		printf("Error: Could not get message data.\n");
		return -1;
	}

	// Compare the info
	ret = strcmp(info.Name(), recvInfo.Name());
	if(ret != 0)
	{
		printf("Error: Sent and received info are different.\n");
		return -1;
	}

	// Compare the acceptance flage
	bool isEqual = accept == recvAccept;	
	if(!isEqual)
	{
		printf("Error: Sent and received acceptance flags are different.\n");
		return -1;
	}

	return 0;
}



int TestUnsubscribeMessage(Sockets::TcpSocket& socket)
{

	int ret;


	// Send the message
	ret = Framer::SendUnsubscribe(socket);
	if(ret != 0)
	{
		printf("Error: Could not send Unsubscribe message.\n");
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
	if(messageType != MessageType::Unsubscribe)
	{
		printf("Error: Received the wrong message type.\n");
		return -1;
	}

	// Get the message data
	ret = Framer::RecvUnsubscribe();
	if(ret != 0)
	{
		printf("Error: Could not get message data.\n");
		return -1;
	}

	return 0;
}


int TestDataParamsMessage(Sockets::TcpSocket& socket, const KinectDataParams& dataParams)
{

	int ret;

	// Create data to use for comparison
	static KinectDataParams recvDataParams;

	// Send the message
	ret = Framer::SendDataParams(socket, dataParams);
	if(ret != 0)
	{
		printf("Error: Could not send DataParams message.\n");
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
	if(messageType != MessageType::DataParams)
	{
		printf("Error: Received the wrong message type.\n");
		return -1;
	}

	// Get the message data
	ret = Framer::RecvDataParams(recvDataParams);
	if(ret != 0)
	{
		printf("Error: Could not get message data.\n");
		return -1;
	}


	// Compare the data params
	bool isEqual = true;
	isEqual &= dataParams.colorImageEnable == recvDataParams.colorImageEnable;
	isEqual &= dataParams.depthImageEnable == recvDataParams.depthImageEnable;
	isEqual &= dataParams.validityImageEnable == recvDataParams.validityImageEnable;
	isEqual &= dataParams.testImageEnable == recvDataParams.testImageEnable;
	
	if(!isEqual)
	{
		printf("Error: Sent and received data params are different.\n");
		return -1;
	}

	return 0;
}



int TestKinectDataMessage(Sockets::TcpSocket& socket, const KinectDataParams& dataParams, const KinectData& kinectData)
{

	int ret;

	// Create data to use for comparison
	static KinectDataParams recvDataParams;
	static KinectData recvKinectData;


	// Send the message
	ret = Framer::SendKinectData(socket, dataParams, kinectData);
	if(ret != 0)
	{
		printf("Error: Could not send KinectData message.\n");
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
	ret = Framer::RecvKinectData(recvDataParams, recvKinectData);
	if(ret != 0)
	{
		printf("Error: Could not get message data.\n");
		return -1;
	}


	// Compare the data params
	bool isEqual = true;
	isEqual &= dataParams.colorImageEnable == recvDataParams.colorImageEnable;
	isEqual &= dataParams.depthImageEnable == recvDataParams.depthImageEnable;
	isEqual &= dataParams.validityImageEnable == recvDataParams.validityImageEnable;
	isEqual &= dataParams.testImageEnable == recvDataParams.testImageEnable;
	
	if(!isEqual)
	{
		printf("Error: Sent and received data params are different.\n");
		return -1;
	}


	// Compare the Kinect data sizes
	isEqual = true;
	
	isEqual &= (dataParams.colorImageEnable ? kinectData.colorImage.rows : 0) == recvKinectData.colorImage.rows;
	isEqual &= (dataParams.colorImageEnable ? kinectData.colorImage.cols : 0) == recvKinectData.colorImage.cols;
	
	isEqual &= (dataParams.depthImageEnable ? kinectData.depthImage.rows : 0) == recvKinectData.depthImage.rows;
	isEqual &= (dataParams.depthImageEnable ? kinectData.depthImage.cols : 0) == recvKinectData.depthImage.cols;
	
	isEqual &= (dataParams.validityImageEnable ? kinectData.validityImage.rows : 0) == recvKinectData.validityImage.rows;
	isEqual &= (dataParams.validityImageEnable ? kinectData.validityImage.cols : 0) == recvKinectData.validityImage.cols;
	
	isEqual &= (dataParams.testImageEnable ? kinectData.testImage.rows : 0) == recvKinectData.testImage.rows;
	isEqual &= (dataParams.testImageEnable ? kinectData.testImage.cols : 0) == recvKinectData.testImage.cols;

	if(!isEqual)
	{
		printf("Error: A sent and received image size is different.\n");
		return -1;
	}


	// Compare the Kinect data values
	isEqual = true;
	if(dataParams.colorImageEnable)
	{
		for(size_t y=0; y<kinectData.colorImage.rows; y++)
		{
			for(size_t x=0; x<kinectData.colorImage.cols; x++)
			{
				isEqual &= kinectData.colorImage.data[y][x].red == recvKinectData.colorImage.data[y][x].red;
				isEqual &= kinectData.colorImage.data[y][x].green == recvKinectData.colorImage.data[y][x].green;
				isEqual &= kinectData.colorImage.data[y][x].blue == recvKinectData.colorImage.data[y][x].blue;
			}
		}
	}

	
	
	if(dataParams.depthImageEnable)
	{
		for(size_t y=0; y<kinectData.depthImage.rows; y++)
		{
			for(size_t x=0; x<kinectData.depthImage.cols; x++)
			{
				isEqual &= kinectData.depthImage.data[y][x] == recvKinectData.depthImage.data[y][x];
			}
		}
	}

	
	if(dataParams.validityImageEnable)
	{
		for(size_t y=0; y<kinectData.validityImage.rows; y++)
		{
			for(size_t x=0; x<kinectData.validityImage.cols; x++)
			{
				isEqual &= kinectData.validityImage.data[y][x] == recvKinectData.validityImage.data[y][x];
			}
		}
	}

	if(dataParams.testImageEnable)
	{
		for(size_t y=0; y<kinectData.testImage.rows; y++)
		{
			for(size_t x=0; x<kinectData.testImage.cols; x++)
			{
				isEqual &= kinectData.testImage.data[y][x] == recvKinectData.testImage.data[y][x];
			}
		}
	}


	if(!isEqual)
	{
		printf("Error: A sent and received image is different.\n");
		return -1;
	}

	return 0;
}



int TestInfoRequestMessage(Sockets::TcpSocket& socket)
{

	int ret;


	// Send the message
	ret = Framer::SendInfoRequest(socket);
	if(ret != 0)
	{
		printf("Error: Could not send InfoRequest message.\n");
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
	if(messageType != MessageType::InfoRequest)
	{
		printf("Error: Received the wrong message type.\n");
		return -1;
	}

	// Get the message data
	ret = Framer::RecvInfoRequest();
	if(ret != 0)
	{
		printf("Error: Could not get message data.\n");
		return -1;
	}

	return 0;
}



int TestInfoMessage(Sockets::TcpSocket& socket, const Network::Info& info)
{

	int ret;

	// Create data to use for comparison
	static Network::Info recvInfo;


	// Send the message
	ret = Framer::SendInfo(socket, info);
	if(ret != 0)
	{
		printf("Error: Could not send SubscribeAck message.\n");
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
	if(messageType != MessageType::Info)
	{
		printf("Error: Received the wrong message type.\n");
		return -1;
	}

	// Get the message data
	ret = Framer::RecvInfo(recvInfo);
	if(ret != 0)
	{
		printf("Error: Could not get message data.\n");
		return -1;
	}

	// Compare the info
	ret = strcmp(info.Name(), recvInfo.Name());
	if(ret != 0)
	{
		printf("Error: Sent and received info are different.\n");
		return -1;
	}


	return 0;
}



}
*/