#ifndef SENSORS__TEST_IMAGE_HOLDER_H
#define SENSORS__TEST_IMAGE_HOLDER_H


// Module Check //
//#ifndef MOD_SENSORS
//#error "Files outside module cannot access file directly."
//#endif


//
// This is a template.
//


// Includes //

// Module


// Project
#include "DataTypes/Images.h"

// Standard C++


// Standard C




// Namespaces //




namespace Sensors
{
	
	class TestImageHolder
	{

	  // PUBLIC //
	  public:
	
		//!!
		static void SetImage(const DataTypes::ColorImage& colorImage);
		static const DataTypes::ColorImage& GetImageReference() { return testImage; }

	  // PRIVATE //
	  private:
		  TestImageHolder() {}
		  virtual ~TestImageHolder() {}

		  //!! Used for the SetTestImage() method
		  static DataTypes::ColorImage testImage;
	};

}


#endif