#ifndef UTIL__DEMO_IMAGE_CREATOR_H
#define UTIL__DEMO_IMAGE_CREATOR_H


// Module Check //
//#ifndef MOD_UTIL
//#error "Files outside module cannot access file directly."
//#endif


//
// This is a template.
//


// Includes //

// Module
#include "Drawing.h"
#include "DataExporter.h"
#include "Helpers.h"

// Project
#include "DataTypes/DataTypes.h"

// Standard C++


// Standard C




// Namespaces //




namespace Util
{
	
	class DemoImageCreator
	{

	  // PUBLIC //
	  public:

		static bool CreateGraphicalImage(ColorImage& colorImage, const KinectData& data)
		{

			ColorImage& testImage = colorImage;

			// Use color image as background
			if(data.available.colorImageEnable)
			{
				// Copy over color image
				data.colorImage.CopyTo(testImage);
			}


			// Create table image
			static BinaryImage tableBlob;
			data.table.CreateTableBlob(tableBlob);
	
	
			// Lighten table
			if(data.available.tableEnable)
			{
		
				// Get indices of points not over the table
				static Types<Point2Di>::FlatImage indices;
				Util::Helpers::GetBlobIndices(tableBlob, indices, true);

				// Darken color image except for over table
				float darkenFactor = 0.25f;
				for(size_t i=0; i<indices.size; i++)
				{
					const Point2Di& point = indices.data[i];
					ColorPixel& colorPixel = testImage.data[point.y][point.x];

					colorPixel.red *= darkenFactor;
					colorPixel.green *= darkenFactor;
					colorPixel.blue *= darkenFactor;
				}
		
			}
	


	
			// Draw arms on image
			if(data.available.handsEnable)
			{
		
				// Draw arm shadows
				for(int i=0; i<data.hands.size(); i++)
				{

			
					// Get arm blob
					static BinaryImage armBlob;
					data.hands[i].CreateArmBlob(armBlob);

					// Get points of arm blob
					static Types<Point2Di>::FlatImage indices;
					Util::Helpers::GetBlobIndices(armBlob, indices);

					// Get shadow shift
					float heightAboveTable = data.table.depth - data.hands[i].meanDepth;
					int xShift = heightAboveTable / 20;
					int yShift = heightAboveTable / 100;

					// Draw shadow
					for(int i=0; i<indices.size; i++)
					{
						const Point2Di& point = indices.data[i];
				
						// Get shifted coordinates
						int xValue = point.x + xShift;
						int yValue = point.y + yShift;

						bool xValueGood = xValue >= 0 && xValue < armBlob.cols;
						bool yValueGood = yValue >= 0 && yValue < armBlob.rows;
						if(xValueGood && yValueGood && !armBlob.data[yValue][xValue])
						{
							ColorPixel& colorPixel = testImage.data[yValue][xValue];
							colorPixel.red = (int)(colorPixel.red * 0.3f);
							colorPixel.green = (int)(colorPixel.green * 0.3f);
							colorPixel.blue = (int)(colorPixel.blue * 0.3f);
						}
					}
			
				}
		
		
				// Draw boundaries around arms
				static const ColorPixel armColors[] = { ColorPixel(ColorPixel::Black), ColorPixel(ColorPixel::Red), ColorPixel(ColorPixel::Green), ColorPixel(ColorPixel::Blue) };
				for(int i=0; i<data.hands.size(); i++)
				{
					const std::vector<Point2Di>& boundary = data.hands[i].boundary;

					if(data.hands[i].id + 1 < sizeof(armColors) / sizeof(ColorPixel));
					{
						for(int j=0; j<boundary.size(); j++)
						{
							const Point2Di& point = boundary[j];
							testImage.data[point.y][point.x] = armColors[data.hands[i].id + 1];
						}
					}
				}
		
		
				// Draw the finger tips
				for(int i=0; i<data.hands.size(); i++)
				{
					const Hand& hand = data.hands[i];
					Util::Drawing::DrawPoints(testImage, hand.fingerTips, ColorPixel(ColorPixel::Green), 3);
				}

				// Draw the finger bases
				for(int i=0; i<data.hands.size(); i++)
				{
					const Hand& hand = data.hands[i];
					Util::Drawing::DrawPoints(testImage, hand.fingerBases, ColorPixel(ColorPixel::Red), 3);
				}

				// Draw the arm base
				for(int i=0; i<data.hands.size(); i++)
				{
					const Hand& hand = data.hands[i];
					if(hand.armBase.x != -1 && hand.armBase.y != -1)
						Util::Drawing::DrawPoints(testImage, hand.armBase, ColorPixel(ColorPixel::Blue), 5);
				}

		
				// Draw the hand palm
				for(int i=0; i<data.hands.size(); i++)
				{
					const Hand& hand = data.hands[i];
					if(hand.palmCenter.x != -1 && hand.palmCenter.y != -1)
						Util::Drawing::DrawPoints(testImage, hand.palmCenter, ColorPixel(255, 0, 255), 5);
				}
		
			}
	

			return data.available.colorImageEnable;
		}


	  

	};

}


#endif
