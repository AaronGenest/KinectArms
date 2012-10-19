//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_DATA_TYPES

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Project


// Standard C++
#include <string>
#include <vector>
#include <array>


// Standard C



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


template<typename T, size_t R, size_t C>
carray2d<T,R,C>::carray2d()
	: data((ArrayType&)*(new T[R*C])), rows(0), cols(0)
{
	return;
}


template<typename T, size_t R, size_t C>
carray2d<T,R,C>::~carray2d()
{
	delete[] (T*)data;
}


template<typename T, size_t R, size_t C>
void carray2d<T,R,C>::Flatten(FlattenedArray& dataArray) const
{
	assert(rows <= maxRows);
	assert(cols <= maxCols);
	assert(rows * cols <= dataArray.maxSize);
		
	const size_t bytesPerRow = cols*sizeof(T);
	for(size_t y=0; y<rows; y++)
	{
		memcpy(&dataArray.data[y*cols], &data[y][0], bytesPerRow);
	}

	dataArray.size = rows*cols;
	return;
}


template<typename T, size_t R, size_t C>
void carray2d<T,R,C>::CopyFrom(const FlattenedArray& dataArray, size_t newRows, size_t newCols)
{
	assert(newRows <= maxRows);
	assert(newCols <= maxCols);
	assert(newRows * newCols == dataArray.size);

	const size_t bytesPerRow = newCols*sizeof(T);
	for(size_t y=0; y<newRows; y++)
	{
		memcpy(&data[y][0], &dataArray.data[y*newCols], bytesPerRow);
	}

	rows = newRows;
	cols = newCols;
	return;
}


template<typename T, size_t R, size_t C>
void carray2d<T,R,C>::CopyTo(carray2d<T,R,C>& dst) const
{
	if(rows == 0 && cols == 0)
	{
		dst.rows = rows;
		dst.cols = cols;
		return;
	}

	memcpy(dst.data, data, sizeof(dst.data));
	dst.rows = rows;
	dst.cols = cols;
	return;
}

//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------
		


}
