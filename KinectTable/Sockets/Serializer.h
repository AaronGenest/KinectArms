#ifndef SOCKETS__SERIALIZER_H
#define SOCKETS__SERIALIZER_H


// Module Check //
#ifndef MOD_SOCKETS
#error "Files outside module cannot access file directly."
#endif



//
// These methods send and receive bytestreams through the network.
//


// Includes //


// Project
#include "DataTypes/DataTypes.h"
#include "Packet.h"
#include "Packetizer.h"

// Standard C
#include <Windows.h>


// Namespaces //


namespace Sockets
{
	
	class Serializer
	{

	  public:

		// Constructors //
		Serializer(const SOCKET& socket);

		// Static Methods //
		Errno SendData(const char data[], unsigned long length);
		Errno ReceiveData(char data[], unsigned long& max_length);

	  private:

		int SendDataLowLevel(const char data[], int length);
		int RecvDataLowLevel(char data[], int& length);


		static const int recvBufferSize = 1000;
		const SOCKET socket;

		Packetizer packetizer;


		Packet sendPacket;
		Packet recvPacket;

		carray<char, recvBufferSize> recvBuffer;
		

	};

}


#endif