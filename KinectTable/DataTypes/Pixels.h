#ifndef DATA_TYPES__PIXELS_H
#define DATA_TYPES__PIXELS_H


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
	
	typedef unsigned short DepthPixel;
	typedef unsigned char GrayPixel;
	typedef bool BinaryPixel;

	typedef struct tagColorPixel
	{

		// Pre-made colours
		enum ColorName
		{
			White,
			Black,
			Red,
			Green,
			Blue
		};
		
		//!!
		tagColorPixel(ColorName name)
		{
			switch(name)
			{
				case White: *this = ColorPixel(255, 255, 255); break;
				case Black: *this = ColorPixel(0, 0, 0); break;
				case Red: *this = ColorPixel(255, 0, 0); break;
				case Green: *this = ColorPixel(0, 255, 0); break;
				case Blue: *this = ColorPixel(0, 0, 255); break;
				default: assert(false);
			}
		}

		tagColorPixel() : red(0), green(0), blue(0)
		{ return; }

		tagColorPixel(GrayPixel red_, GrayPixel green_, GrayPixel blue_)
			: red(red_), green(green_), blue(blue_)
		{ return; }

		tagColorPixel& operator=(const tagColorPixel& color)
		{
			if(this == &color)
				return *this;

			red = color.red;
			green = color.green;
			blue = color.blue;

			return *this;
		}

		bool operator==(const tagColorPixel& color)
		{
			return (red == color.red) && (green == color.green) && (blue == color.blue);
		}

		GrayPixel red;
		GrayPixel green;
		GrayPixel blue;

	  private:
		/*static const tagColorPixel White;
		static const tagColorPixel Red;
		static const tagColorPixel Green;
		static const tagColorPixel Blue;*/

	} ColorPixel;

	
}


#endif