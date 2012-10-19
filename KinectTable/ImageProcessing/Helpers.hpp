
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Project
#include "DataTypes/Images.h"
#include "DataTypes/Points.h"

// Standard C++
#include <exception>

// Standard C
#include <stdlib.h>
#include <assert.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------


namespace ImageProcessing
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

template<typename T>
void Crop(typename const DataTypes::Types<T>::Image& input, const DataTypes::Point2Di& min, const DataTypes::Point2Di& max, typename DataTypes::Types<T>::Image& output)
{

	assert(0 <= min.x);
	assert(min.x <= max.x);
	assert(max.x < input.cols);

	assert(0 <= min.y);
	assert(min.y <= max.y);
	assert(max.y < input.rows);

	assert(input.rows <= output.maxRows);
	assert(input.cols <= output.maxCols);

	output.rows = max.y - min.y + 1;
	output.cols = max.x - min.x + 1;
	for(int y=0; y<output.rows; y++)
	{
		for(int x=0; x<output.cols; x++)
		{
			output.data[y][x] = input.data[y + min.y][x + min.x];
		}
	}


	return;
}

//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------



}
