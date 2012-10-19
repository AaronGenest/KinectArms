#ifndef SERIALIZERS__CARRAY_SERIALIZER_H
#define SERIALIZERS__CARRAY_SERIALIZER_H


// Module Check //
#ifndef MOD_SERIALIZERS
#error "Files outside module cannot access file directly."
#endif


//
// This holds all the image pixel types.
//


// Includes //

// Module


// Project
#include "DataTypes/carray.h"
#include "Serialization\Serialization.h"

// Standard C++


// Standard C
#include <assert.h>



// Namespaces //




namespace DataTypes
{

	template<typename T, size_t S>
	struct carraySerializer : public Serialization::ISerializable
	{

		carraySerializer(carray<T,S>& data) : data(data)
		{ return; }

		void Serialize(Serialization::Serializer& serializer) const
		{
			
			serializer.WriteList(data.data, data.size);
			return;
		}

		void Deserialize(Serialization::Deserializer& deserializer)
		{
			unsigned long length = data.maxSize;
			deserializer.ReadList(data.data, length);
			data.size = length;
			return;
		}

	  private:
		carray<T,S>& data;

	};

}


#endif