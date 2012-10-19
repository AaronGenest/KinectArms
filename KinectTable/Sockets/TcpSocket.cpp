//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SOCKETS


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "TcpSocket.h"

// Project
#include "Serializer.h"

// Standard C++


// Standard C
#include <Windows.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Sockets
{

//---------------------------------------------------------------------------
// Static Public Methods
//---------------------------------------------------------------------------



TcpSocket* TcpSocket::Connect(const char address[], unsigned short port)
{
	int ret;

	// Initialize the Winsock library
	ret = InitWinsock();
	if(ret != 0)
		throw -1;

	// Set up socket
	SOCKET tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcpSocket == INVALID_SOCKET)
		throw -1;

	// Set up address info
	struct sockaddr_in addressInfo;
	addressInfo.sin_family = AF_INET;
	addressInfo.sin_addr.s_addr = inet_addr(address);
	addressInfo.sin_port = htons(port);

	ret = ConnectSocket(tcpSocket, addressInfo);

	return new TcpSocket(tcpSocket, addressInfo);
}


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


TcpSocket::~TcpSocket()
{
	//!! Shutdown socket?

	// Close socket
	(void)closesocket(tcpSocket);

	// Release WinSock
	(void)DeInitWinsock();
	return;
}




int TcpSocket::SendData(const char data[], unsigned long length)
{
	if(!isConnected)
		return -1;

	// Send the data
	int ret = 0;
	ret = serializer.SendData(data, length);
	if(ret != 0)
		return -1;
	
	return 0;
}


int TcpSocket::ReceiveData(char data[], unsigned long& length)
{
	if(!isConnected)
		return -1;

	// Receive data
	Errno ret = serializer.ReceiveData(data, length);
	if(ret != 0)
		return -1;

	return 0;
}



void TcpSocket::GetAddress(IpAddress address) const
{
	memcpy(address, this->address, sizeof(address));
	return;
}

//---------------------------------------------------------------------------
// Protected Methods
//---------------------------------------------------------------------------

TcpSocket::TcpSocket(const SOCKET& tcpSocket, const struct sockaddr_in& addressInfo)
	: addressInfo(addressInfo), port(addressInfo.sin_port), tcpSocket(tcpSocket), isConnected(true),
	  serializer(tcpSocket)
{

	// Get address
	char* address =  inet_ntoa(addressInfo.sin_addr);
	memcpy(this->address, address, sizeof(this->address));

	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


int TcpSocket::InitWinsock()
{
	WSADATA wsaData;
	int ret;
	
	// Initialize Winsock
	ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (ret != 0)
		return -1;

	return 0;
}

int TcpSocket::DeInitWinsock()
{
	int ret = WSACleanup();
	if(ret == SOCKET_ERROR)
		return WSAGetLastError();

	return 0;
}


int TcpSocket::ConnectSocket(SOCKET& tcpSocket, const sockaddr_in& addressInfo)
{
	int ret;

	// Make connection request to server
	ret = connect(tcpSocket, (SOCKADDR*)&addressInfo, sizeof(addressInfo));
	if(ret == SOCKET_ERROR)
        return -1;

	return 0;
}


}
