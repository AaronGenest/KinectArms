#ifndef SOCKETS__TCP_LISTENER_H
#define SOCKETS__TCP_LISTENER_H


// Module Check //
#ifndef MOD_SOCKETS
#error "Files outside module cannot access file directly."
#endif



//
// This class is a listening socket using the TCP protocol.
//


// Includes //


// Project
#include "TcpSocket.h"


// Standard C++


// Standard C
#include <Windows.h>
//#include <winsock2.h>


// Namespaces //




namespace Sockets
{
	
	class TcpListener
	{

	  public:

		// Data //

		static const int addressLength = 16;
		typedef char IpAddress[addressLength];


		// Constructors //

		TcpListener(const IpAddress address, unsigned short port);
		virtual ~TcpListener();


		// Mutators //

		int StartListening();
		int StopListening();

		TcpSocket* Accept();


		// Accessors //

		bool IsListening() const { return isListening; }
		unsigned short GetPort() const { return port; }
		void GetAddress(IpAddress address) const;
	

	  private:
	
		static int InitWinsock();
		static int DeInitWinsock();

		int Bind();
		int Listen();
	

		IpAddress address;
		const unsigned short port;

		struct sockaddr_in addressInfo;
		SOCKET tcpSocket;

		bool isListening;

	};
	
}


#endif