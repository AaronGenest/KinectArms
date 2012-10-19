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
void Deserializer::ReadVector(std::vector<T>& elements)
{
	// Make sure T implements the ISerializable interface
	//dynamic_cast<ISerializable*>(&T());

	unsigned long length;
	this->ReadPrimitive(length);

	elements.resize(length);
	assert(elements.size() == length);

	T* elems = elements.data();
	for(int i=0; i<length; i++)
	{
		Read(elems[i]);
	}

	return;
}

template<typename T>
void Deserializer::ReadList(T* objs, unsigned long& length)
{
	// Get length
	{
		unsigned long objsNum;
		this->ReadPrimitive(objsNum);
		assert(length >= objsNum);
		length = objsNum;
	}

	for(int i=0; i<length; i++)
	{
		this->Read(objs[i]);
	}

	return;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


template<typename T>
void Deserializer::ReadPrimitive(T& value)
{
	// Make sure T is a primitive
	assert(sizeof(T) <= 4);

	// Make sure there's enough room
	assert(BytesLeft() >= sizeof(T));

	T& intValue = (T&)bytes[byteNum];
	value = intValue;
	
	byteNum += sizeof(T);
	return;
}

/*
template<typename T>
void Deserializer::ReadPrimitiveList(T* values, unsigned long& length)
{
	unsigned long numBytes;
	this->ReadPrimitive(numBytes);

	assert(numBytes <= length*sizeof(T));
	std::memcpy(values, &bytes[byteNum], numBytes);

	byteNum += numBytes;
	length = numBytes;
	return;
}
*/


}
