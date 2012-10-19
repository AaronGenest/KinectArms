#ifndef UTIL__SERIALIZERS_H
#define UTIL__SERIALIZERS_H

// Includes //

// Module
#include "Serialization/Serialization.h"

// Project
#include "DataTypes/DataTypes.h"

// Standard C++
#include <vector>

// Namespaces //


namespace Util
{
	template<typename T>
	class Serializer
	{

	  public:

		  static void Serialize(const std::vector<T>& elements, char data[], unsigned long& maxSize);
		  static void Deserialize(const char data[], unsigned long& maxSize, std::vector<T>& elements);

	};
}

// Template files
#include "Serializer.hpp"

#endif