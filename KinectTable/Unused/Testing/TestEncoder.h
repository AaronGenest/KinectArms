#ifndef TEST_ENCODER_H
#define TEST_ENCODER_H


// Includes //
#include "DataTypes.h"

// Namespaces //


namespace Testing
{
	
	// Methods //
	
	template<typename T>
	void TestEncoder(typename const Types<T>::Image& image);
	
}


#include "TestEncoder.hpp"


#endif