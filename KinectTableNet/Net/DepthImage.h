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
	/// Holds the size and data of a depth image.
	/// </summary>
	public ref class DepthImage sealed : public ImageFrame
	{
	  
	  public:

		/// <summary>
		/// Creates a depth image.  All memory is allocated.
		/// </summary>
		DepthImage()
		{
			width = 0;
			height = 0;
			bytesPerPixel = bytesPerDepthPixel;

			bytes = gcnew array<Byte>(DataTypes::DepthImage::maxRows * DataTypes::DepthImage::maxCols * bytesPerPixel);
			
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

		  
		DepthImage(const DataTypes::DepthImage& image)
		{
			width = image.cols;
			height = image.rows;
			bytesPerPixel = bytesPerDepthPixel;

			bytes = gcnew array<Byte>(image.rows * image.cols * bytesPerPixel);
			Marshal::Copy(IntPtr(image.data), bytes, 0, bytes->Length);
			/*
			for(int y=0; y<image.rows; y++)
			{
				for(int x=0; x<image.cols; x++)
				{
					DataTypes::DepthPixel& pixel = image.data[y][x];

					int byteNum = (y*image.cols + x) * bytesPerPixel;
					bytes[byteNum + 0] = (Byte)(pixel & 0x00FF);
					bytes[byteNum + 1] = (Byte)((pixel & 0xFF00) >> 8);
					bytes[byteNum + 2] = 0;
					bytes[byteNum + 3] = 0;
				}
			}
			*/
			return;
		}

		void CopyFrom(const DataTypes::DepthImage& image)
		{
			width = image.cols;
			height = image.rows;
			bytesPerPixel = bytesPerDepthPixel;

			Marshal::Copy(IntPtr(image.data), bytes, 0, bytes->Length);
			return;
		}


		static const int bytesPerDepthPixel = 2;

	};

}
