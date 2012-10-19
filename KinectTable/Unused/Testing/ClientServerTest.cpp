
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "ClientServerTest.h"

// Project
//#include "NetworkServer.h"
//#include "NetworkClient.h"
#include "TcpSocket.h"


// Standard C++


// Standard C
#include <stdio.h>
#include <assert.h>
#include <time.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------


//using namespace NetworkFramer;
//using namespace Network;
namespace ClientServerTest
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

const char serverAddress[] = "136.159.14.7";
const unsigned short serverPortNum = 55555;

//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------
int RunServer();
int RunClient();
template<typename T>
void CreateRandomImage(typename Types<T>::Image& image);
//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------
int RunClientServerTest()
{

	int ret;

	printf("Starting Client/Server test...\n\n");

	
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
	/*
 	SessionType sessionType = SessionType::SESSION_TYPE_PUBLIC;

	// Creating data params
	KinectDataParams dataParams;
	dataParams.colorImageEnable = true;
	dataParams.depthImageEnable = true;
	dataParams.validityImageEnable = false;
	dataParams.tableImageEnable = true;

	// Creating data
	printf("Creating Kinect data...\n");
	KinectData kinectData;
	CreateRandomImage<ColorPixel>(kinectData.colorImage);
	CreateRandomImage<DepthPixel>(kinectData.depthImage);
	CreateRandomImage<BinaryPixel>(kinectData.validityImage);
	CreateRandomImage<BinaryPixel>(kinectData.tableImage);
	printf("Kinect Data created.\n\n");
	

	NetworkServer::InitializeServer(serverAddress, serverPortNum, sessionType, dataParams);

	
	bool testdone = false;
	bool connectionaccepted = false;

	printf("Server Running.\n");
	while(!testdone){
		if(NetworkServer::AcceptClients())
		{
			//Someone connected!
			connectionaccepted = true;
		}
		if(NetworkServer::ReceiveClientMessages() != 0)
		{
			//Message error!
			printf("quitting...\n");
			return 1;
		}
		if(!NetworkServer::SendDataToClients(kinectData))
		{
			//Send failed
			printf("quitting...\n");
			return 1;
		}
		

		if(NetworkServer::getHasUnsubscribed())
		{
			
			testdone = true;
		}
	}
	if(connectionaccepted)
		return 0;
	else
		return -1;

		*/
	return 0;
}

int RunClient()
{
	/*
	SessionType sessionType = SessionType::SESSION_TYPE_PUBLIC;

	KinectDataParams dataParams;
	dataParams.colorImageEnable = true;
	dataParams.depthImageEnable = true;
	dataParams.validityImageEnable = false;
	dataParams.tableImageEnable = true;

	NetworkClient::InitializeClient();

	NetworkClient::ConnectToServer(serverAddress, serverPortNum, sessionType, dataParams);

	NetworkClient::ReceiveDataFromServer();
	
	NetworkClient::SendUnsubscribe(NetworkClient::getServer());
	*/
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

}

