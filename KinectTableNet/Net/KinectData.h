#pragma once

// Unmanaged //
#include "DataTypes\KinectData.h"
#include "DataTypes\KinectDataParams.h"

// Managed //
#include "KinectDataParams.h"
#include "ColorImage.h"
#include "DepthImage.h"
#include "BinaryImage.h"
#include "Table.h"
#include "Hand.h"

// Namespaces //
using namespace System;

namespace KinectTableNet
{
	
	/// <summary>
	/// Holds all the data from KinectTable.
	/// </summary>
	public ref class KinectData sealed
	{
	  
	  public:

		/// <summary>
		/// Creates a new data structure.  All memory for images are allocated.
		/// </summary>
		KinectData()
		{
			// Available
			available = gcnew KinectDataParams(KinectDataParams::EnableType::None);

			// Images
			colorImage = gcnew KinectTableNet::ColorImage();
			depthImage = gcnew KinectTableNet::DepthImage();
			validityImage = gcnew KinectTableNet::BinaryImage();
			testImage = gcnew KinectTableNet::ColorImage();
			
			return;
		}

		/// <summary>
		/// The data that is available.
		/// </summary>
		property KinectDataParams^ Available
		{
			KinectDataParams^ get() { return available; }
		}


		// Images //

		/// <summary>
		/// The color image from the Kinect.
		/// </summary>
		property ImageFrame^ ColorImage
		{
			ImageFrame^ get() { return colorImage; }
		}

		/// <summary>
		/// The depth image from the Kinect.
		/// </summary>
		property ImageFrame^ DepthImage
		{
			ImageFrame^ get() { return depthImage; }
		}

		/// <summary>
		/// A binary image showing the valid pixels from the depth image.
		/// </summary>
		property ImageFrame^ ValidityImage
		{
			ImageFrame^ get() { return validityImage; }
		}

		/// <summary>
		/// An image created by the KinectTable library.  Used for debugging.
		/// </summary>
		property ImageFrame^ TestImage
		{
			ImageFrame^ get() { return testImage; }
		}


		/// <summary>
		/// The info for the table in the image.
		/// </summary>
		property Table^ TableInfo
		{
			Table^ get() { return table; }
		}
		
		/// <summary>
		/// The info for the hands in the image.
		/// </summary>
		property array<Hand^>^ Hands
		{
			array<Hand^>^ get() { return hands; }
		}


		void CopyTo(KinectData^ kinectData)
		{
			// Available
			kinectData->available = available;
			
			// Images
			colorImage->CopyTo(kinectData->colorImage);
			depthImage->CopyTo(kinectData->depthImage);
			validityImage->CopyTo(kinectData->validityImage);
			testImage->CopyTo(kinectData->testImage);

			// Data
			table->CopyTo(kinectData->table);
			kinectData->hands = gcnew array<Hand^>(hands->Length);
			for(int i=0; i<kinectData->hands->Length; i++)
			{
				hands[i]->CopyTo(kinectData->hands[i]);
			}

			return;
		}

	  internal:

		KinectData(const DataTypes::KinectData& kinectData)
		{
			// Available
			available = gcnew KinectDataParams(kinectData.available);

			// Images
			if(available->colorImageEnable)
				colorImage = gcnew KinectTableNet::ColorImage(kinectData.colorImage);
			else
				colorImage = nullptr;

			if(available->depthImageEnable)
				depthImage = gcnew KinectTableNet::DepthImage(kinectData.depthImage);
			else
				depthImage = nullptr;

			if(available->validityImageEnable)
				validityImage = gcnew KinectTableNet::BinaryImage(kinectData.validityImage);
			else
				validityImage = nullptr;

			if(available->testImageEnable)
				testImage = gcnew KinectTableNet::ColorImage(kinectData.testImage);
			else
				testImage = nullptr;

			// Table
			table = gcnew Table(kinectData.table);


			// Hand
			hands = gcnew array<Hand^>(kinectData.hands.size());
			for(int i=0; i<kinectData.hands.size(); i++)
			{
				hands[i] = gcnew Hand(kinectData.hands[i]);
			}

			return;
		}


		/// <summary>
		/// Copies an unmanaged array into a managed array without reallocating any image data.
		/// </summary>
		void CopyFrom(const DataTypes::KinectData& kinectData)
		{
			// Available
			available = gcnew KinectDataParams(kinectData.available);

			// Images
			if(available->colorImageEnable)
			{
				if(colorImage == nullptr)
					colorImage = gcnew KinectTableNet::ColorImage(kinectData.colorImage);
				else
					colorImage->CopyFrom(kinectData.colorImage);
			}

			if(available->depthImageEnable)
			{
				if(depthImage == nullptr)
					depthImage = gcnew KinectTableNet::DepthImage(kinectData.depthImage);
				else
					depthImage->CopyFrom(kinectData.depthImage);
			}

			if(available->validityImageEnable)
			{
				if(validityImage == nullptr)
					validityImage = gcnew KinectTableNet::BinaryImage(kinectData.validityImage);
				else
					validityImage->CopyFrom(kinectData.validityImage);
			}
			
			if(available->testImageEnable)
			{
				if(testImage == nullptr)
					testImage = gcnew KinectTableNet::ColorImage(kinectData.testImage);
				else
					testImage->CopyFrom(kinectData.testImage);
			}
			

			// Table
			table = gcnew Table(kinectData.table);

			// Hand
			hands = gcnew array<Hand^>(kinectData.hands.size());
			for(int i=0; i<kinectData.hands.size(); i++)
			{
				hands[i] = gcnew Hand(kinectData.hands[i]);
			}

			return;
		}

	  private:

		KinectDataParams^ available;
		KinectTableNet::ColorImage^ colorImage;
		KinectTableNet::DepthImage^ depthImage;
		KinectTableNet::BinaryImage^ validityImage;

		KinectTableNet::ColorImage^ testImage;
		Table^ table;
		array<Hand^>^ hands;

	};

}
