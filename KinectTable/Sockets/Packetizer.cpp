//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SOCKETS

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Packetizer.h"

// Project


// Standard C++


// Standard C
#include <assert.h>
//#include <WinSock2.h>
#include <Windows.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------



namespace Sockets
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void Packetizer::AddBytes(const char bytes[], int length)
{
	// Make sure we have enough room in the buffer
	assert(byteBuffer.size - bufferStart + length <= byteBuffer.maxSize);

	// Move bytes to the beginning of the byte buffer
	byteBuffer.size = byteBuffer.size - bufferStart;
	memmove(byteBuffer.data, &byteBuffer.data[bufferStart], byteBuffer.size);
	bufferStart = 0;

	// Copy new bytes into byte buffer
	memcpy(&byteBuffer.data[byteBuffer.size], bytes, length);
	byteBuffer.size += length;

	return;
}


int Packetizer::FindPacket(Packet& userPacket)
{
	// Overlay the packet structure on top of the byte buffer
	const Packet& packet = *(Packet*)&byteBuffer.data[bufferStart];


	// Check for the header
	if(byteBuffer.size - bufferStart < packet.headerSize)
		return -1;

	// Check the magic number
	if(packet.MagicNum() != packet.magicNumValue)
		return -2;


	// Get the packet size
	const size_t packetSize = packet.headerSize + ntohl(packet.DataLength());

	// Make sure we have enough data
	if(byteBuffer.size - bufferStart < packetSize)
		return -1;

	// Give packet to user
	memcpy(&userPacket, &packet, packetSize);

	// Update the starting of the byte buffer
	bufferStart += packetSize;

	return 0;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------





}
