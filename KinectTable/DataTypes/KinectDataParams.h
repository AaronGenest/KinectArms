#ifndef DATA_TYPES__KINECT_DATA_PARAMS_H
#define DATA_TYPES__KINECT_DATA_PARAMS_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif


//
// This holds all the image pixel types.
//


// Includes //


// Project
#include "Serialization/Serialization.h"


// Standard C++


// Standard C
#include <assert.h>


// Namespaces //




namespace DataTypes
{
	
	struct __declspec(dllexport) KinectDataParams : public Serialization::ISerializable
	{
		
		enum EnableType
		{
			All,
			None,
			AllButImages,
			ImagesOnly
		};


		KinectDataParams();
		KinectDataParams(EnableType enableType);


		// Images
		bool colorImageEnable;
		bool depthImageEnable;
		bool validityImageEnable;
		bool testImageEnable;

		// Data
		bool tableEnable;
		bool handsEnable;


		bool operator==(const KinectDataParams& params) const;
		bool operator!=(const KinectDataParams& params) const;

		size_t SizeOf() const { return sizeof(KinectDataParams); }


		void Serialize(Serialization::Serializer& serializer) const;
		void Deserialize(Serialization::Deserializer& deserializer);


	  private:
		
		void InitFromEnableType(EnableType enableType);

	};


}


#endif