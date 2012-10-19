#ifndef SERIALIZERS__CARRAY2D_SERIALIZER_H
#define SERIALIZERS__CARRAY2D_SERIALIZER_H


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
#include "DataTypes/carray2d.h"
#include "Serialization\Serialization.h"


//Standard C++


//Standard C
#include <stdlib.h>
#include <assert.h>


// Namespaces //




namespace DataTypes
{
	

	template<typename T, size_t R, size_t C>
	struct carray2dSerializer : public Serialization::ISerializable
	{
		carray2dSerializer(carray2d<T,R,C>& data) : data(data)
		{ return; }

		void Serialize(Serialization::Serializer& serializer) const
		{

			serializer.Write(this->rows);

			for(size_t y=0; y<rows; y++)
			{
				serializer.WriteList(&(this->data[y][0]), this->cols);
			}
			
			return;
		}

		void Deserialize(Serialization::Deserializer& deserializer)
		{
			deserializer.Read(this->rows);

			unsigned long length = this->maxCols;
			for(size_t y=0; y<rows; y++)
			{
				deserializer.ReadList(&(this->data[y][0]), length);
			}

			this->cols = length;
			return;
		}

	  private:
		carray2d<T,R,C>& data;

	};


}


#endif