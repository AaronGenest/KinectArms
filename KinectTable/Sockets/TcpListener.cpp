//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SOCKETS

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "TcpListener.h"

// Project


// Standard C++


// Standard C
#include <Windows.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Sockets
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

TcpListener::TcpListener(const IpAddress address, unsigned short port) : port(port), isListening(false)
{
	int ret;

	// Copy parameters
	memcpy(this->address, address, sizeof(this->address));

	// Initialize the Winsock library
	ret = InitWinsock();
	if(ret != 0)
		throw -1;

	// Set up socket
	tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcpSocket == INVALID_SOCKET)
		throw -1;

	// Set up address info
	addressInfo.sin_family = AF_INET;
	addressInfo.sin_addr.s_addr = inet_addr(address);
	addressInfo.sin_port = htons(port);

	return;
}

TcpListener::~TcpListener()
{
	if(isListening)
		(void)StopListening();

	(void)DeInitWinsock();
	return;
}

int TcpListener::StartListening()
{
	if(isListening)
		return 0;

	int ret;

	// Bind socket
	ret = Bind();
	if(ret != 0)
		return -1;

	// Start listening on socket
	ret = Listen();
	if(ret != 0)
		return -1;

	isListening = true;
	return 0;
}

int TcpListener::StopListening()
{
	if(!isListening)
		return 0;

	(void)closesocket(tcpSocket);

	isListening = false;
	return 0;
}



TcpSocket* TcpListener::Accept()
{
	if(!isListening)
		return NULL;
	
	int addressLength = sizeof(addressInfo);
	SOCKET newSocket = accept(tcpSocket, (SOCKADDR*)&addressInfo, &addressLength);
	if(newSocket == INVALID_SOCKET)
		return NULL;

	return new TcpSocket(newSocket, addressInfo);
}




void TcpListener::GetAddress(IpAddress address) const
{
	memcpy(address, this->address, sizeof(address));
	return;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


int TcpListener::InitWinsock()
{
	WSADATA wsaData;
	int ret;
	
	// Initialize Winsock
	ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (ret != 0)
		return -1;

	return 0;
}

int TcpListener::DeInitWinsock()
{
	int ret = WSACleanup();
	if(ret == SOCKET_ERROR)
		return WSAGetLastError();

	return 0;
}




int TcpListener::Bind()
{

	int ret = bind(tcpSocket, (SOCKADDR*)&addressInfo, sizeof(addressInfo));
	if (ret == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		return err;
	}

	return 0;
}

int TcpListener::Listen()
{

	int ret = listen(tcpSocket, SOMAXCONN);
	if(ret == SOCKET_ERROR )
		return -1;

	return 0;
}


}
