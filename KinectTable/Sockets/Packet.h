#ifndef SOCKETS__PACKET_H
#define SOCKETS__PACKET_H


// Module Check //
#ifndef MOD_SOCKETS
#error "Files outside module cannot access file directly."
#endif



//
// This is a network packet used by the Serializer
//


// Includes //


// Project


// Standard C++


// Standard C
#include <assert.h>



// Namespaces //




namespace Sockets
{
	
	

	// Data Declarations //

	struct Packet
	{
		static const int maxDataLength = 10000;  //maximum number of bytes of data per packet
		static const char magicNumValue = (char)0xAF;
		static const int headerSize = sizeof(char) + sizeof(unsigned char) + sizeof(unsigned long);

		typedef char DataArrayType[maxDataLength];
		typedef char ByteArrayType[headerSize + maxDataLength];

		Packet()
		{
			assert(sizeof(MagicNum()) == 1);
			assert(sizeof(PacketNum()) == 1);
			assert(sizeof(DataLength()) == 4);
			
			byteArray[0] = magicNumValue;
			return;
		}

		const char& MagicNum() const { return byteArray[0]; }  //is used as a check to make sure we are looking at the start of a packet
		
		unsigned char& PacketNum() { return *(unsigned char*)&byteArray[1]; }
		const unsigned char& PacketNum() const { return *(unsigned char*)&byteArray[1]; }

		unsigned long& DataLength() { return *(unsigned long*)&byteArray[2]; }
		const unsigned long& DataLength() const { return *(const unsigned long*)&byteArray[2]; }

		DataArrayType& Data() { return (DataArrayType&)byteArray[6]; }
		const DataArrayType& Data() const { return (const DataArrayType&)byteArray[6]; }
		
	  private:
		ByteArrayType byteArray;
	  
	};


}


#endif