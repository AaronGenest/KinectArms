#ifndef DATA_TYPES__CONSTANTS_H
#define DATA_TYPES__CONSTANTS_H


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



// Namespaces //




namespace DataTypes
{
	
	// Constants //

	const size_t COLOR_RES_X = 640;
	const size_t COLOR_RES_Y = 480;
	const size_t COLOR_DEPTH = 3;

	const size_t DEPTH_RES_X = 640;
	const size_t DEPTH_RES_Y = 480;

	const size_t MAX_DEPTH_VALUE = 8191;  //Max value of 13 bits
	const size_t INVALID_DEPTH_VALUE = 0;

	const size_t IMAGE_ROWS = 480;
	const size_t IMAGE_COLS = 640;
	const size_t DATA_SIZE = IMAGE_ROWS*IMAGE_COLS*2; //multiply by 2 so that we can store an image and any extra data we want

	const size_t MAX_HANDS = 10;

}


#endif