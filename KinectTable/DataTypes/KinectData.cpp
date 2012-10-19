//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_DATA_TYPES

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "KinectData.h"

// Module


// Project


// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace DataTypes
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void KinectData::CopyTo(tagKinectData& data) const
{
	// Data params
	data.available = available;

	// Raw Images
	colorImage.CopyTo(data.colorImage);
	depthImage.CopyTo(data.depthImage);
	validityImage.CopyTo(data.validityImage);
		
	// Test Image
	testImage.CopyTo(data.testImage);

	// Table Data
	data.table = table;

	// Arms Data
	data.hands = hands;

	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------





}
