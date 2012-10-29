#pragma once

// Unmanaged //
#include "DataTypes\Table.h"

// Managed //
#include "ColorImage.h"
#include "DepthImage.h"
#include "BinaryImage.h"

// Namespaces //
using namespace System;
using namespace System::Drawing;

namespace KinectTableNet
{
	
	/// <summary>
	/// Holds information about a hand in the image.
	/// </summary>
	public ref class Hand sealed
	{
	  
	  public:

		/// <summary>
		/// The ID number of the hand.
		/// </summary>
		property unsigned char Id
		{
			unsigned char get() { return id; }
		}

		/// <summary>
		/// The point where the arm intersects with the edge of the table.
		/// </summary>
		property Point ArmBase
		{
			Point get() { return armBase; }
		}

		/// <summary>
		/// The center of the palm.
		/// </summary>
		property Point PalmCenter
		{
			Point get() { return palmCenter; }
		}

		/// <summary>
		/// The points corresponding to the boundary of the arm.
		/// </summary>
		property array<Point>^ Boundary
		{
			array<Point>^ get() { return boundary; }
		}

		/// <summary>
		/// The tips of each finger.
		/// </summary>
		property array<Point>^ FingerTips
		{
			array<Point>^ get() { return fingerTips; }
		}

		/// <summary>
		/// The point between each finger.
		/// </summary>
		property array<Point>^ FingerBases
		{
			array<Point>^ get() { return fingerBases; }
		}

		/// <summary>
		/// The average depth of the arm.
		/// </summary>
		property float MeanDepth
		{
			float get() { return meanDepth; }
		}

		/// <summary>
		/// The area in pixels of the arm.
		/// </summary>
		property int Area
		{
			int get() { return area; }
		}

		/// <summary>
		/// The geometric centroid of the arm.
		/// </summary>
		property Point Centroid
		{
			Point get() { return centroid; }
		}


		/// <summary>
		/// Creates a binary image of the area of the arm.  The boundary is shown in white.
		/// </summary>
		ImageFrame^ CreateArmBlob()
		{
			return CreateArmBlob(true);
		}

		/// <summary>
		/// Creates a binary image of the area of the arm.  The area is shown in white.
		/// </summary>
		/// <param name="fillBlob">Whether or not to fill the region.</param>
		ImageFrame^ CreateArmBlob(bool fillBlob)
		{
			if(fillBlob)
				return filledArmBlob;
			else
				return armBlob;
		}


	  void CopyTo(Hand^ hand)
	  {
		hand->id = id;
		
		hand->armBase = armBase;
		hand->palmCenter = palmCenter;

		hand->boundary = boundary;
		hand->fingerTips = fingerTips;
		hand->fingerBases = fingerBases;

		// Arm properties
		hand->meanDepth = meanDepth;
		hand->area = area;
		hand->centroid = centroid;

		armBlob->CopyTo(hand->armBlob);
		filledArmBlob->CopyTo(hand->filledArmBlob);

		return;
	  }

	  internal:

		Hand(const DataTypes::Hand& hand)
		{
			
			// Save table structure
			/*
			DataTypes::Hand& thisHand = *(this->handU);
			//thisTable = table;
			thisHand.id = hand.id;
			thisHand.armBase = hand.armBase;
			thisHand.boundary = hand.boundary;
			thisHand.fingerTips = hand.fingerTips;
			thisHand.fingerBases = hand.fingerBases;

			thisHand.palmCenter = hand.palmCenter;

			thisHand.meanDepth = hand.meanDepth;
			thisHand.area = hand.area;
			thisHand.centroid = hand.centroid;
			*/

			DataTypes::BinaryImage binaryImage;
			hand.CreateArmBlob(binaryImage, false);
			armBlob = gcnew BinaryImage(binaryImage);

			hand.CreateArmBlob(binaryImage, true);
			filledArmBlob = gcnew BinaryImage(binaryImage);


			// Get id
			id = hand.id;

			// Get arm base and palm center
			armBase = Point(hand.armBase.x, hand.armBase.y);
			palmCenter = Point(hand.palmCenter.x, hand.palmCenter.y);



			// Get boundary, finger tips, and finger bases
			boundary = CreatePointArray(hand.boundary);
			fingerTips = CreatePointArray(hand.fingerTips);
			fingerBases = CreatePointArray(hand.fingerBases);

			// Get arm properties
			meanDepth = hand.meanDepth;
			area = hand.area;
			centroid = Point(hand.centroid.x, hand.centroid.y);

			return;
		}

		~Hand()
		{
			/*
			if(handU != NULL)
			{
				delete handU;
				handU = NULL;
			}

			if(binaryImageU != NULL)
			{
				delete binaryImageU;
				binaryImageU = NULL;
			}
			*/
			return;
		}

	  private:


		array<Point>^ CreatePointArray(const std::vector<Point2Di>& points)
		{
			array<Point>^ pointArray = gcnew array<Point>(points.size());
			for(int i=0; i<points.size(); i++)
			{
				const DataTypes::Point2Di& point = points[i];
				pointArray[i] = Point(point.x, point.y);
			}

			return pointArray;
		}


		unsigned char id;
		
		Point armBase;
		Point palmCenter;

		array<Point>^ boundary;
		array<Point>^ fingerTips;
		array<Point>^ fingerBases;

		// Arm properties
		float meanDepth;
		int area;
		Point centroid;  // Center of geometry calculated from boundary points

		ImageFrame^ armBlob;
		ImageFrame^ filledArmBlob;


		//DataTypes::Hand* handU;
		

	};

}
