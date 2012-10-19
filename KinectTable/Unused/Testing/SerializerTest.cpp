
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "SerializerTest.h"

// Project
#include "Serializer.h"
#include "DataTypes.h"

// Standard C++


// Standard C
#include <stdio.h>
#include <assert.h>
#include <time.h>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace SerializerTest
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

// Number of bytes to send
const int dataLength = 500000;

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


int RunSerializerTest()
{

	int ret;

	printf("Starting Serializer test...\n\n");

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


	SOCKET socket listeningSocket;

	// Bind
	printf("Binding socket...\n");
	printf("\tAddress: %s\n", address);
	printf("\tPort Num: %hu\n", portNum);
	ret = bind(listeningSocket, address, portNum);
	if(ret != 0)
	{
		printf("Error: Could not bind.\n");
		return -1;
	}
	printf("Socket is bound.\n\n");

	// Listen
	printf("Listening on socket...\n");
	ret = listeningSocket.Listen();
	if(ret != 0)
	{
		printf("Error: Could not listen.\n");
		return -1;
	}
	printf("Listened on socket.\n\n");

	// Accept
	Network::TcpSocket connectionSocket;
	printf("Accepting a socket...\n");
	ret = listeningSocket.Accept(connectionSocket);
	if(ret != 0)
	{
		printf("Error: Could not accept a socket.\n");
		return -1;
	}
	printf("Accepted a socket.\n\n");
	

	// Receive data
	printf("Waiting to receive data...\n");
	static carray<char, dataLength> data;
	int receivedBytes = 0;
	const int totalBytes = data.maxSize;
	while(receivedBytes < totalBytes)
	{
		printf("Receiving data...\n\n");
		int recvLength = data.maxSize - receivedBytes;
		ret = connectionSocket.ReceiveData(&data.data[receivedBytes], recvLength);
		if(ret != 0)
		{
			printf("Error: Could not receive data.\n");
			return -1;
		}

		receivedBytes += recvLength;
		printf("Received data (%lu of %lu).\n\n", receivedBytes, totalBytes);
	}

	
	// Send data
	printf("Sending data...\n");
	ret = connectionSocket.SendData(data.data, data.maxSize);
	if(ret != 0)
	{
		printf("Error: Could not send data.\n");
		return -1;
	}
	printf("Data is sent.\n\n");


	return 0;
}


int RunClient()
{
	int ret;


	Network::TcpSocket connectionSocket;

	// Bind
	printf("Connecting socket...\n");
	printf("\tAddress: %s\n", address);
	printf("\tPort Num: %hu\n", portNum);
	ret = connectionSocket.Connect(address, portNum);
	if(ret != 0)
	{
		printf("Error: Could not connect.\n");
		return -1;
	}
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
	

	// Send data
	printf("Sending data...\n");
	ret = connectionSocket.SendData(sendData.data, sendData.size);
	if(ret != 0)
	{
		printf("Error: Could not send data.\n");
		return -1;
	}
	printf("Data is sent.\n\n");


	// Receive data
	printf("Waiting to receive data...\n");
	static carray<char, dataLength> recvData;
	int receivedBytes = 0;
	const int totalBytes = recvData.maxSize;
	while(receivedBytes < totalBytes)
	{
		printf("Receiving data...\n\n");
		int recvLength = recvData.maxSize - receivedBytes;
		ret = connectionSocket.ReceiveData(&recvData.data[receivedBytes], recvLength);
		if(ret != 0)
		{
			printf("Error: Could not receive data.\n");
			return -1;
		}

		receivedBytes += recvLength;
		printf("Received data (%lu of %lu).\n\n", receivedBytes, totalBytes);
	}
	

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


	return 0;
}



}
