//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "Serializer.h"

// Project
#include "DataTypes/DataTypes.h"

// Standard C++
#include <vector>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace std;
using namespace DataTypes;

namespace Util
{

//---------------------------------------------------------------------------
// Data Type Declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

template<typename T>
void Serializer<T>::Serialize(const std::vector<T>& elements, char data[], unsigned long& maxSize)
{
	// Make sure T implements the ISerializable interface
	dynamic_cast<ISerializable*>(&T());
	

	unsigned long byteNum = 0;

	// Set number of elements
	unsigned long& size = (unsigned long&)data[byteNum];
	size = htonl(elements.size());
	byteNum += sizeof(unsigned long);
	
	// Set elements
	for(int i=0; i<elements.size(); i++)
	{
		const ISerializable& element = elements[i];

		unsigned long bytesLeft = maxSize - byteNum;
		element.Serialize(&data[byteNum], bytesLeft);
		byteNum += bytesLeft;
	}
	
	return;
}

template<typename T>
void Serializer<T>::Deserialize(const char data[], unsigned long& maxSize, std::vector<T>& elements)
{
	// Make sure T implements the ISerializable interface
	dynamic_cast<ISerializable*>(&T());

	unsigned long byteNum = 0;

	// Get size of data
	unsigned long& size = (unsigned long&)data[byteNum];
	unsigned long numElements = ntohl(size);
	byteNum += sizeof(unsigned long);

	// Resize vector
	elements.resize(numElements);

	// Get elements
	for(int i=0; i<elements.size(); i++)
	{
		ISerializable& element = elements[i];

		unsigned long bytesLeft = maxSize - byteNum;
		element.Deserialize(&data[byteNum], bytesLeft);
		byteNum += bytesLeft;
	}

	maxSize = byteNum;
	return;
}

}