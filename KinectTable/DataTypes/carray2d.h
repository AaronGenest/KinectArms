#ifndef DATA_TYPES__CARRAY2D_H
#define DATA_TYPES__CARRAY2D_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif



//
// This holds all the image pixel types.
//


// Includes //

// Module
#include "carray.h"

// Project


//Standard C++


//Standard C
#include <stdlib.h>
#include <assert.h>


// Namespaces //




namespace DataTypes
{
	
	template<typename T, size_t R, size_t C>
	struct __declspec(dllexport) carray2d
	{

		typedef T Type;
		typedef T ArrayType[R][C];
		typedef carray<T,R*C> FlattenedArray;

		// We dynamically allocate so we don't have to worry about stack overflow problems
		carray2d();
		~carray2d();



		ArrayType& data;
		size_t rows;
		size_t cols;

		static const size_t typeSize = sizeof(T);
		static const size_t maxRows = R;
		static const size_t maxCols = C;



		void Flatten(FlattenedArray& dataArray) const;

		void CopyFrom(const FlattenedArray& dataArray, size_t newRows, size_t newCols);


		size_t SizeOf() const { return sizeof(size_t) + maxRows*maxCols*typeSize; }
	
		void CopyTo(carray2d<T,R,C>& dst) const;



	  private:
		
		carray2d<T,R,C>& operator=(const carray2d<T,R,C> other) { throw ENOTSUP; }
	};


}

#include "carray2d.hpp"

#endif