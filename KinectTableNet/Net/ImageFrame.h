#pragma once


// Unmanaged
#include "DataTypes\Images.h"

// Namespaces //
using namespace System;
//using namespace System::Windows::Media::Imaging;
using namespace System::Drawing;


namespace KinectTableNet
{
	
	/// <summary>
	/// Holds the size and data of an image.
	/// </summary>	
	public ref class ImageFrame abstract
	{
	  
	  public:

		/// <summary>
		/// Width of the image in pixels.
		/// </summary>
		int width;

		/// <summary>
		/// Height of the image in pixels.
		/// </summary>
		int height;

		/// <summary>
		/// The number of bytes per pixel.
		/// </summary>
		int bytesPerPixel;

		/// <summary>
		/// The data of the image as an array of bytes.
		/// </summary>
		property array<Byte>^ Bytes
		{
			array<Byte>^ get() { return bytes; }
			void set(array<Byte>^ bytes) { this->bytes = bytes; }
		}
		
		//virtual Bitmap^ AsBitmap() = 0;
		//virtual BitmapSource AsBitmapSource() = 0;

		void CopyTo(ImageFrame^ imageFrame)
		{
			imageFrame->width = width;
			imageFrame->height = height;
			imageFrame->bytesPerPixel = bytesPerPixel;

			bytes->CopyTo(imageFrame->bytes, 0);
			return;
		}
	  

	  protected:

		array<Byte>^ bytes;

	};

}
