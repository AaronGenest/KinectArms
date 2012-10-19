#ifndef SOCKETS__PACKETIZER_H
#define SOCKETS__PACKETIZER_H


// Module Check //
#ifndef MOD_SOCKETS
#error "Files outside module cannot access file directly."
#endif



//
// This class finds packets in a stream of bytes.
//


// Includes //


// Project
#include "DataTypes/DataTypes.h"
#include "Packet.h"

// Standard C++


// Standard C




// Namespaces //



namespace Sockets
{
	
	class Packetizer
	{
	
	  public:
	
		Packetizer()
		{
			byteBuffer.size = 0;
			bufferStart = 0;
			return;
		}

		//NOTE: Do not add bytes unless the FindPacket() function tells you to!
		void AddBytes(const char bytes[], int length);

		// Looks for a packet in its byte buffer.
		// Returns 0 if packet is found.
		// Returns -1 if needs more data.
		// Returns -2 if packets are corrupted or out of sync.
		int FindPacket(Packet& packet);


	  private:

		carray<char, Packet::maxDataLength * 2> byteBuffer;
		size_t bufferStart;

	};

}


#endif
