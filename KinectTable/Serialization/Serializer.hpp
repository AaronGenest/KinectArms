//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SERIALIZATION

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Module


// Project


// Standard C++


// Standard C
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


template<typename T>
void Serializer::WriteList(const T* objs, unsigned long length)
{

	this->WritePrimitive(length);

	for(size_t i=0; i<length; i++)
	{
		this->Write(objs[i]);
	}

	return;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


template<typename T>
void Serializer::WritePrimitive(T value)
{
	// Make sure T is a primitive
	assert(sizeof(T) <= 4);

	// Make sure there's enough room
	assert(BytesLeft() >= sizeof(T));

	T& intValue = (T&)bytes[byteNum];
	intValue = value;
			
	byteNum += sizeof(T);
	return;
}

/*
template<typename T>
void Serializer::WritePrimitiveList(const T* values, unsigned long length)
{
	this->WritePrimitive(length);

	unsigned long numBytes = length*sizeof(T);
	assert(BytesLeft() >= numBytes);
	std::memcpy(&bytes.data()[byteNum], values, numBytes);

	byteNum += numBytes;
	return;
}
*/

}
