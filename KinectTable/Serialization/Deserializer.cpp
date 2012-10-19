//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SERIALIZATION

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Deserializer.h"

// Module


// Project


// Standard C++


// Standard C
#include <errno.h>
#include <WinSock2.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Serialization
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void Deserializer::Reset()
{
	byteNum = 0;
	return;
}


void Deserialize(Deserializer& deserializer, ISerializable& obj)
{
	obj.Deserialize(deserializer);
	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------

void Deserializer::ReadByteList(char* values, unsigned long& length)
{
	unsigned long numBytes;
	this->ReadPrimitive(numBytes);

	assert(numBytes <= BytesLeft());
	assert(numBytes <= length*sizeof(char));
	std::memcpy(values, &bytes[byteNum], numBytes);

	byteNum += numBytes;
	length = numBytes;
	return;
}

void Deserializer::ReadShortList(short* values, unsigned long& length)
{
	unsigned long recvLength;
	this->ReadPrimitive(recvLength);

	unsigned long numBytes = length * sizeof(short);
	assert(numBytes <= BytesLeft());

	assert(recvLength <= length);
	length = recvLength;

	const short* shorts = (const short*)bytes;
	for(unsigned long i=0; i<length; i++)
		values[i] = ntohs(shorts[i]);
	
	byteNum += numBytes;
	return;
}

void Deserializer::ReadLongList(long* values, unsigned long& length)
{
	unsigned long recvLength;
	this->ReadPrimitive(recvLength);

	unsigned long numBytes = length * sizeof(long);
	assert(numBytes <= BytesLeft());

	assert(recvLength <= length);
	length = recvLength;

	const long* longs = (const long*)bytes;
	for(unsigned long i=0; i<length; i++)
		values[i] = ntohl(longs[i]);
	
	byteNum += numBytes;
	return;
}


short Deserializer::ConvertShort(short value)
{
	return ntohs(value);
}

long Deserializer::ConvertLong(long value)
{
	return ntohl(value);
}


}
