#ifndef DATA_TYPES__IMAGES_H
#define DATA_TYPES__IMAGES_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif


//
// This holds all the image pixel types.
//


// Includes //


// Project
#include "Constants.h"
#include "carray.h"
#include "carray2d.h"
#include "Pixels.h"

// Standard C++

// Standard C



// Namespaces //




namespace DataTypes
{
	


template<typename T>
struct Types
{
	
	typedef carray2d<T, IMAGE_ROWS, IMAGE_COLS> Image;
	typedef typename Image::FlattenedArray FlatImage;
	typedef typename FlatImage::ByteArray FlatImageBytes;


	//typedef T ImageData[IMAGE_ROWS][IMAGE_COLS];
	//typedef T FlatImageData[IMAGE_ROWS*IMAGE_COLS*sizeof(T)];
	//typedef T DataStreamData[DATA_SIZE];
};


typedef Types<GrayPixel>::Image GrayImage;
typedef Types<ColorPixel>::Image ColorImage;
typedef Types<DepthPixel>::Image DepthImage;
typedef Types<BinaryPixel>::Image BinaryImage;
typedef carray<char, DATA_SIZE> ByteStream;

//typedef GrayPixel GrayImage[COLOR_RES_Y][COLOR_RES_X];
//typedef ColorPixel ColorImage[COLOR_RES_Y][COLOR_RES_X];
//typedef DepthPixel DepthImage[DEPTH_RES_Y][DEPTH_RES_X];
//typedef BinaryPixel BinaryImage[DEPTH_RES_Y][DEPTH_RES_X];


}


#endif