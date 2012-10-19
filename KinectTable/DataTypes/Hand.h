#ifndef DATA_TYPES__HAND_H
#define DATA_TYPES__HAND_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif


//
// This holds all the image pixel types.
//


// Includes //


// Project
#include "Points.h"
#include "Images.h"
#include "Serialization/Serialization.h"
#include "Randomization/Randomization.h"


// Standard C++
#include <vector>

// Standard C
#include <assert.h>



// Namespaces //



namespace DataTypes
{
	
	struct __declspec(dllexport) Hand : public Serialization::ISerializable, public Randomization::IRandomizable
	{

		// Takes a HandPimpl as parameter
		Hand();
		Hand(const Hand& hand);
		virtual ~Hand();
		
		int id; //specifies the id of the hand

		Point2Di armBase; // Base of the arm (where the arm intersects the edge of the table)
		std::vector<Point2Di> boundary;
		std::vector<Point2Di> fingerTips; //!! Should give depth
		std::vector<Point2Di> fingerBases; //!! Should give depth

		Point2Di palmCenter;
		//!!Point2Di palmNormal;

		// Setting fillBlob to false will give you just the border
		virtual void CreateArmBlob(BinaryImage& armBlob, bool fillBlob = true) const;

		void* const handPimpl;

		// Arm properties
		float meanDepth;
		int area;
		Point2Di centroid;  // Center of geometry calculated from boundary points


		Hand& operator=(const Hand& hand);
		
		bool operator==(const Hand& hand) const;
		bool operator!=(const Hand& hand) const;
		
		void Serialize(Serialization::Serializer& serializer) const;
		void Deserialize(Serialization::Deserializer& deserializer);
		

		void Randomize(Randomization::Randomizer& randomizer)
		{

			randomizer.Randomize(id);
			
			randomizer.Randomize(armBase);
			randomizer.RandomizeVector(boundary);
			randomizer.RandomizeVector(fingerTips);
			randomizer.RandomizeVector(fingerBases);

			randomizer.Randomize(palmCenter);


			// Arm properties
			randomizer.Randomize(meanDepth);
			randomizer.Randomize(area);
			randomizer.Randomize(centroid);

			return;
		}
		  
	};


}


#endif