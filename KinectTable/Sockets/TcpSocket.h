#ifndef SOCKETS__TCP_SOCKET_H
#define SOCKETS__TCP_SOCKET_H


// Module Check //
#ifndef MOD_SOCKETS
#error "Files outside module cannot access file directly."
#endif



//
// This class represents a socket using the TCP protocol.
//


// Includes //


// Project
#include "Serializer.h"


// Standard C++


// Standard C
#include <Windows.h>



// Namespaces //


namespace Sockets
{

	//!!Needs to set its connected flag to false if we get an error that the socket is disconnected
	class TcpSocket
	{

	  public:

		static TcpSocket* Connect(const char address[], unsigned short port);


		// Data //

		static const int addressLength = 16;
		typedef char IpAddress[addressLength];


		// Constructors //

		virtual ~TcpSocket();


		// Mutators //
	
		int SendData(const char data[], unsigned long length);
		int ReceiveData(char data[], unsigned long& length);


		// Accessors //

		bool IsConnected() const { return isConnected; }
		unsigned short GetPort() const;
		void GetAddress(IpAddress address) const;
	

	  protected:
	
		TcpSocket(const SOCKET& tcpSocket, const struct sockaddr_in& addressInfo);


	  private:
	
		static int InitWinsock();
		static int DeInitWinsock();
		static int ConnectSocket(SOCKET& tcpSocket, const sockaddr_in& addressInfo);
	
		IpAddress address;
		const unsigned short port;

		struct sockaddr_in addressInfo;
		const SOCKET tcpSocket;

		Serializer serializer;

		bool isConnected;

		friend class TcpListener;
	};
	
}


#endif