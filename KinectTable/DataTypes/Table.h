#ifndef DATA_TYPES__TABLE_H
#define DATA_TYPES__TABLE_H


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
	
	struct __declspec(dllexport) Table : public Serialization::ISerializable, public Randomization::IRandomizable
	{
	
		std::vector<Point2Di> corners;

		DepthPixel depth;

		//!! Normal Vector
		//!! Corners in 3D space relative to the Kinect
		//!! Area of table
		//!! Center of geometry

		

		void CreateTableBlob(BinaryImage& tableBlob, bool fillBlob = true) const;
		


		Table& operator=(const Table& table);

		bool operator==(const Table& table) const;
		bool operator!=(const Table& table) const;


		void Serialize(Serialization::Serializer& serializer) const;
		void Deserialize(Serialization::Deserializer& deserializer);
		
		void Randomize(Randomization::Randomizer& randomizer)
		{
			const int maxCorners = 10;
			
			randomizer.Randomize(depth);
			
			corners.resize(rand() % maxCorners);
			randomizer.RandomizeVector(corners);

			return;
		}

	};


}


#endif