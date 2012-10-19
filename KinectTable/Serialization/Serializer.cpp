//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SERIALIZATION

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Serializer.h"

// Module


// Project


// Standard C++


// Standard C
#include <WinSock2.h>
#include <stdlib.h>
#include <errno.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Serialization
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void Serializer::Reset()
{
	byteNum = 0;
	return;
}

unsigned long Serializer::GetBytes(unsigned char data[], unsigned long maxSize)
{
	if(maxSize < byteNum)
		throw -1;

	memcpy(data, bytes.data(), byteNum);
	return byteNum;
}



void Serialize(Serializer& serializer, const ISerializable& obj)
{
	obj.Serialize(serializer);
	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


void Serializer::WriteByteList(const char* values, unsigned long length)
{
	this->WritePrimitive(length);

	unsigned long numBytes = length*sizeof(char);
	assert(BytesLeft() >= numBytes);
	std::memcpy(&bytes.data()[byteNum], values, numBytes);

	byteNum += numBytes;
	return;
}

void Serializer::WriteShortList(const short* values, unsigned long length)
{
	this->WritePrimitive(length);

	unsigned long numBytes = length*sizeof(short);
	assert(BytesLeft() >= numBytes);

	short* shorts = (short*)bytes.data();
	for(unsigned long i=0; i<length; i++)
		shorts[i] = htons(values[i]);

	byteNum += numBytes;
	return;
}

void Serializer::WriteLongList(const long* values, unsigned long length)
{
	this->WritePrimitive(length);

	unsigned long numBytes = length*sizeof(long);
	assert(BytesLeft() >= numBytes);

	long* longs = (long*)bytes.data();
	for(unsigned long i=0; i<length; i++)
		longs[i] = htonl(values[i]);

	byteNum += numBytes;
	return;
}


short Serializer::ConvertShort(short value)
{
	return htons(value);
}


long Serializer::ConvertLong(long value)
{
	return htonl(value);
}


}
