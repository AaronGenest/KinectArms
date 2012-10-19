
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "SocketTest.h"

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




namespace SocketTest
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


int RunSocketTest()
{

	int ret;

	printf("Starting Socket test...\n\n");

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
	

	while(true)
	{

		// Receive data
		printf("Receiving data...\n");
		static carray<char, dataLength> data;
		unsigned long recvLength = data.maxSize;

		ret = socket.ReceiveData(data.data, recvLength);
		if(ret != 0)
		{
			printf("Error: Could not receive data.\n");
			return -1;
		}
		data.size = recvLength;
		printf("Received data.\n\n");

	
		// Send data
		printf("Sending data...\n");
		ret = socket.SendData(data.data, data.size);
		if(ret != 0)
		{
			printf("Error: Could not send data.\n");
			return -1;
		}
		printf("Data is sent.\n\n");

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
	static carray<char, dataLength> sendData;
	for(size_t i=0; i<sendData.maxSize; i++)
	{
		sendData.data[i] = (char)(rand() % 0xFF);
	}
	sendData.size = sendData.maxSize;
	printf("Data prepared.\n\n");
	

	while(true)
	{

		// Send data
		printf("Sending data...\n");
		ret = socket.SendData(sendData.data, sendData.size);
		if(ret != 0)
		{
			printf("Error: Could not send data.\n");
			return -1;
		}
		printf("Data is sent.\n\n");


		// Receive data
		printf("Receiving data...\n");
		static carray<char, dataLength> recvData;
		unsigned long recvLength = recvData.maxSize;

		ret = socket.ReceiveData(recvData.data, recvLength);
		if(ret != 0)
		{
			printf("Error: Could not receive data.\n");
			return -1;
		}
		recvData.size = recvLength;
		printf("Received data.\n\n");
	

		// Compare the sent data with the received data
		printf("Comparing sent data to received data...\n");
		assert(sendData.size == recvData.size);
		for(size_t i=0; i<sendData.size; i++)
		{
			if(sendData.data[i] != recvData.data[i])
			{
				printf("Error: Sent data and received data are different.\n");
				return -1;
			}
		}
		printf("Sent data and received data are the same.\n\n");

	}

	return 0;
}



}
