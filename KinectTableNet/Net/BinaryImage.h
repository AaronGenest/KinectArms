#pragma once

// Unmanaged
#include "DataTypes\Images.h"

// Managed
#include "ImageFrame.h"

// Namespaces //
using namespace System;
//using namespace System::Windows::Media::Imaging;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;

namespace KinectTableNet
{
	
	/// <summary>
	/// Holds the size and data of a binary image.  (Pixels can only be white or black)
	/// </summary>
	public ref class BinaryImage sealed : public ImageFrame
	{

	  public:
		
		/// <summary>
		/// Creates a binary image.  All memory is allocated.
		/// </summary>
		BinaryImage()
		{
			width = 0;
			height = 0;
			bytesPerPixel = bytesPerBinaryPixel;

			bytes = gcnew array<Byte>(DataTypes::BinaryImage::maxRows * DataTypes::BinaryImage::maxCols * bytesPerPixel);
			
			return;
		}
	  
	  internal:

		  
		BinaryImage(const DataTypes::BinaryImage& image)
		{
			width = image.cols;
			height = image.rows;
			bytesPerPixel = bytesPerBinaryPixel;

			bytes = gcnew array<Byte>(image.rows * image.cols * bytesPerPixel);
			Marshal::Copy(IntPtr(image.data), bytes, 0, bytes->Length);
			
			return;
		}

		void CopyFrom(const DataTypes::BinaryImage& image)
		{
			width = image.cols;
			height = image.rows;
			bytesPerPixel = bytesPerBinaryPixel;

			Marshal::Copy(IntPtr(image.data), bytes, 0, bytes->Length);			
			return;
		}

		static const int bytesPerBinaryPixel = 1;

	};

}
