#ifndef DATA_TYPES__CARRAY_H
#define DATA_TYPES__CARRAY_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif


//
// This holds all the image pixel types.
//


// Includes //


// Project

// Standard C++


// Standard C
#include <assert.h>



// Namespaces //




namespace DataTypes
{
	
	template<typename T, size_t S>
	struct __declspec(dllexport) carray
	{

		typedef T Type;
		typedef T ArrayType[S];
		typedef carray<char, S*sizeof(T)> ByteArray;

		// We dynamically allocate so we don't have to worry about stack overflow problems
		carray();
		~carray();


		ArrayType& data;
		size_t size;
	
		static const size_t typeSize = sizeof(T);
		static const size_t maxSize = S;

	
		void CopyTo(carray<T,S>& dst) const;


		size_t SizeOf() const { return sizeof(size_t) + maxSize * typeSize; }



	  private:

		carray<T,S>& operator=(const carray<T,S> other) { throw ENOTSUP; }

	};

}


#include "carray.hpp"

#endif