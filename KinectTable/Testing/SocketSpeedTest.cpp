//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "SocketSpeedTest.h"

// Project
#include "DataTypes/DataTypes.h"
#include "Sockets/Sockets.h"


// Standard C++


// Standard C
#include <stdio.h>
#include <assert.h>
#include <time.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace SocketSpeedTest
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

// Number of bytes to send
const unsigned long dataLength = 500000;

const char address[] = "127.0.0.1";
const unsigned short portNum = 55555;


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------

int RunServer();
int RunClient();


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


int RunSocketSpeedTest()
{

	int ret;

	printf("Starting Socket speed test...\n\n");

	// Assert some pre-test conditions
	assert(sizeof(dataLength) == sizeof(int));

	
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
	carray<char, dataLength> data;
	while(true)
	{

		// Receive data
		unsigned long recvLength = sizeof(data.data);
		ret = socket.ReceiveData(data.data, recvLength);
		if(ret != 0)
		{
			printf("Error: Could not receive data.\n");
			return -1;
		}

	
		// Send data
		ret = socket.SendData(data.data, sizeof(data));
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

	// Create data to send
	printf("Preparing data to send...\n");
	carray<char, dataLength> data;
	for(size_t i=0; i<sizeof(data.data); i++)
	{
		data.data[i] = (char)(rand() % 0xFF);
	}
	printf("Data prepared.\n\n");
	

	// Send/Receive data
	printf("Sending/Receiving data...\n");
	size_t frameNum = 0;
	size_t seconds = 0;
	unsigned long firstMilliseconds = GetTickCount();
	while(true)
	{
		unsigned long startMilliseconds = GetTickCount();

		// Send data
		ret = socket.SendData(data.data, sizeof(data.data));
		if(ret != 0)
		{
			printf("Error: Could not send data.\n");
			return -1;
		}


		// Receive data
		unsigned long recvLength = sizeof(data.data);
		ret = socket.ReceiveData(data.data, recvLength);
		if(ret != 0)
		{
			printf("Error: Could not receive data.\n");
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
			const size_t bitsPerFrame = sizeof(data) * 8;
			printf("Data sent and received.\n");
			printf("\tAve FPS: %g   -   Now FPS: %g\n", aveFps, fps);
			printf("\tAve kbps: %g   -   Now kbps: %g\n", aveFps * bitsPerFrame / (double)1000, fps * bitsPerFrame / (double)1000);
			
			seconds = elapsedSeconds;
		}
	
	}

	return 0;
}



}
