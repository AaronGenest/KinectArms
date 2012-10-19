//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_DATA_TYPES

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "KinectDataParams.h"

// Project


// Standard C++


// Standard C
#include <errno.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace DataTypes
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

KinectDataParams::KinectDataParams()
{
	InitFromEnableType(None);
	return;
}

KinectDataParams::KinectDataParams(EnableType enableType)
{
	InitFromEnableType(enableType);
	return;
}


bool KinectDataParams::operator==(const KinectDataParams& params) const
{
	bool isEqual = true;

	// Images
	isEqual &= (colorImageEnable == params.colorImageEnable);
	isEqual &= (depthImageEnable == params.depthImageEnable);
	isEqual &= (validityImageEnable == params.validityImageEnable);
	isEqual &= (testImageEnable == params.testImageEnable);

	// Data
	isEqual &= (tableEnable == params.tableEnable);
	isEqual &= (handsEnable == params.handsEnable);

	return isEqual;
}


bool KinectDataParams::operator!=(const KinectDataParams& params) const
{
	return !(*this == params);
}


void KinectDataParams::Serialize(Serialization::Serializer& serializer) const
{

	char byte = 0;
	byte |= colorImageEnable ? 0x01 : 0x00;
	byte |= depthImageEnable ? 0x02 : 0x00;
	byte |= validityImageEnable ? 0x04 : 0x00;
	byte |= testImageEnable ? 0x08 : 0x00;
			
	byte |= tableEnable ? 0x10 : 0x00;
	byte |= handsEnable ? 0x20 : 0x00;

	serializer.Write(byte);
	return;
}


void KinectDataParams::Deserialize(Serialization::Deserializer& deserializer)
{
	char byte;
	deserializer.Read(byte);
			
	colorImageEnable = (byte & 0x01) == 0x01;
	depthImageEnable = (byte & 0x02) == 0x02;
	validityImageEnable = (byte & 0x04) == 0x04;
	testImageEnable = (byte & 0x08) == 0x08;

	tableEnable = (byte & 0x10) == 0x10;
	handsEnable = (byte & 0x20) == 0x20;

	return;
}

//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------
		

void KinectDataParams::InitFromEnableType(EnableType enableType)
{
	switch(enableType)
	{
		case All:
		{
			// Images
			colorImageEnable = true;
			depthImageEnable = true;
			validityImageEnable = true;
			testImageEnable = true;

			// Data
			tableEnable = true;
			handsEnable = true;
			break;
		}

		case None:
		{
			// Images
			colorImageEnable = false;
			depthImageEnable = false;
			validityImageEnable = false;
			testImageEnable = false;

			// Data
			tableEnable = false;
			handsEnable = false;
			break;
		}

		case AllButImages:
		{
			// Images
			colorImageEnable = false;
			depthImageEnable = false;
			validityImageEnable = false;
			testImageEnable = false;

			// Data
			tableEnable = true;
			handsEnable = true;
			break;
		}

		case ImagesOnly:
		{
			// Images
			colorImageEnable = true;
			depthImageEnable = true;
			validityImageEnable = true;
			testImageEnable = true;

			// Data
			tableEnable = false;
			handsEnable = false;
			break;
		}

		default: assert(false); throw ENOTSUP;
	}

	return;
}


}
