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
	/// Holds the size and data of a color image.
	/// </summary>
	public ref class ColorImage sealed : public ImageFrame
	{
	  
	  public:

		/// <summary>
		/// Creates a color image.  All memory is allocated.
		/// </summary>
		ColorImage()
		{
			width = 0;
			height = 0;
			bytesPerPixel = bytesPerColorPixel;

			bytes = gcnew array<Byte>(DataTypes::ColorImage::maxRows * DataTypes::ColorImage::maxCols * bytesPerPixel);
			
			return;
		}

		/*
		override System::Drawing::Bitmap^ AsBitmap()
		{
			Bitmap bitmap = gcnew Bitmap(width, height, PixelFormat::Format32bppRgb);
			BitmapData data = bitmap.LockBits(gcnew Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, bitmap.PixelFormat);
			IntPtr dataPtr = data.Scan0;

			const int bytesPerPixel = 4;
			Marshal::Copy(bytes, 0, dataPtr, width * height * bytesPerPixel);
			return bitmap;
		}
		*/
		/*
		BitmapSource AsBitmapSource()
		{
			throw -1;
		}
		*/

	  internal:

		  
		ColorImage(const DataTypes::ColorImage& image)
		{
			width = image.cols;
			height = image.rows;
			bytesPerPixel = bytesPerColorPixel;

			bytes = gcnew array<Byte>(image.rows * image.cols * bytesPerPixel);
			for(int y=0; y<image.rows; y++)
			{
				for(int x=0; x<image.cols; x++)
				{
					DataTypes::ColorPixel& pixel = image.data[y][x];

					int byteNum = (y*image.cols + x) * bytesPerPixel;
					bytes[byteNum + 0] = pixel.blue;
					bytes[byteNum + 1] = pixel.green;
					bytes[byteNum + 2] = pixel.red;
					//bytes[byteNum + 3] = 0;
				}
			}
			
			return;
		}

		
		void CopyFrom(const DataTypes::ColorImage& image)
		{
			width = image.cols;
			height = image.rows;
			bytesPerPixel = bytesPerColorPixel;

			for(int y=0; y<image.rows; y++)
			{
				for(int x=0; x<image.cols; x++)
				{
					DataTypes::ColorPixel& pixel = image.data[y][x];

					int byteNum = (y*image.cols + x) * bytesPerPixel;
					bytes[byteNum + 0] = pixel.blue;
					bytes[byteNum + 1] = pixel.green;
					bytes[byteNum + 2] = pixel.red;
					//bytes[byteNum + 3] = 0;
				}
			}
			
			return;
		}

		static const int bytesPerColorPixel = 3;

	};

}
