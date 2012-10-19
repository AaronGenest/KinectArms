#ifndef NETWORK__KINECT_DATA_SERIALIZER_H
#define NETWORK__KINECT_DATA_SERIALIZER_H


// Module Check //
#ifndef MOD_NETWORK
#error "Files outside module cannot access file directly."
#endif


//
// This holds all the image pixel types.
//


// Includes //

// Module
#include "ImageSerializer.h"

// Project
#include "DataTypes/DataTypes.h"
#include "Serialization/Serialization.h"


// Standard C++


// Standard C
#include <assert.h>



// Namespaces //




namespace Network
{
	
	struct KinectDataSerializer : Serialization::ISerializable
	{

	  

		KinectDataSerializer(KinectData& data, ImageSerializer<ColorPixel>& colorImageSerializer, ImageSerializer<DepthPixel>& depthImageSerializer, ImageSerializer<BinaryPixel>& binaryImageSerializer)
			: data(data), colorImageSerializer(colorImageSerializer), depthImageSerializer(depthImageSerializer), binaryImageSerializer(binaryImageSerializer)
		{ return; }

		KinectData& data;


		void Serialize(Serialization::Serializer& serializer) const
		{


			// Set data parameters in frame //
			serializer.Write(data.available);
			

			// Set images in frame //

			// Color Image
			if(data.available.colorImageEnable)
			{

				// Convert values to network byte order
				assert(sizeof(data.colorImage.data[0][0].red) == 1);
				assert(sizeof(data.colorImage.data[0][0].green) == 1);
				assert(sizeof(data.colorImage.data[0][0].blue) == 1);

				// Set image data
				colorImageSerializer.SerializeImage(data.colorImage, serializer);
			}



			// Depth Image
			if(data.available.depthImageEnable)
			{

				// Convert values to network byte order
				//!! Shouldn't be touching user's data!
				assert(data.depthImage.typeSize == sizeof(short));
				for(size_t y=0; y<data.depthImage.rows; y++)
				{
					for(size_t x=0; x<data.depthImage.cols; x++)
					{
						data.depthImage.data[y][x] = htons(data.depthImage.data[y][x]);
					}
				}

				// Set image data
				depthImageSerializer.SerializeImage(data.depthImage, serializer);

				// Convert user's values back to host byte order
				assert(data.depthImage.typeSize == sizeof(short));
				for(size_t y=0; y<data.depthImage.rows; y++)
				{
					for(size_t x=0; x<data.depthImage.cols; x++)
					{
						data.depthImage.data[y][x] = ntohs(data.depthImage.data[y][x]);
					}
				}
			}



			// Validity Image
			if(data.available.validityImageEnable)
			{
				// Convert values to network byte order
				assert(data.validityImage.typeSize == 1);
		
				// Set image data
				binaryImageSerializer.SerializeImage(data.validityImage, serializer);
			}



			// Test Image
			if(data.available.testImageEnable)
			{
				// Convert values to network byte order
				assert(sizeof(data.testImage.data[0][0].red) == 1);
				assert(sizeof(data.testImage.data[0][0].green) == 1);
				assert(sizeof(data.testImage.data[0][0].blue) == 1);


				// Set image data
				colorImageSerializer.SerializeImage(data.testImage, serializer);
			}
	

			// Table
			if(data.available.tableEnable)
			{
				serializer.Write(data.table);
			}

			// Hands
			if(data.available.handsEnable)
			{
				serializer.WriteVector(data.hands);
			}

			return;
		}

		void Deserialize(Serialization::Deserializer& deserializer)
		{
			
			// Get data parameters //
			deserializer.Read(data.available);

			// Get images //

			// Color Image
			if(data.available.colorImageEnable)
			{

				// Get image data
				colorImageSerializer.DeserializeImage(deserializer, data.colorImage);

				// Convert values to network byte order
				assert(sizeof(data.colorImage.data[0][0].red) == 1);
				assert(sizeof(data.colorImage.data[0][0].green) == 1);
				assert(sizeof(data.colorImage.data[0][0].blue) == 1);

			}



			// Depth Image
			if(data.available.depthImageEnable)
			{
				// Get image data
				depthImageSerializer.DeserializeImage(deserializer, data.depthImage);

				// Convert values to network byte order
				assert(data.depthImage.typeSize == sizeof(short));
				for(size_t y=0; y<data.depthImage.rows; y++)
				{
					for(size_t x=0; x<data.depthImage.cols; x++)
					{
						data.depthImage.data[y][x] = ntohs(data.depthImage.data[y][x]);
					}
				}

			}

			// Validity Image
			if(data.available.validityImageEnable)
			{
				// Get image data
				binaryImageSerializer.DeserializeImage(deserializer, data.validityImage);

				// Convert values to network byte order
				assert(data.validityImage.typeSize == 1);
			}

			// Test Image
			if(data.available.testImageEnable)
			{
				// Get image data
				colorImageSerializer.DeserializeImage(deserializer, data.testImage);

				// Convert values to network byte order
				assert(sizeof(data.testImage.data[0][0].red) == 1);
				assert(sizeof(data.testImage.data[0][0].green) == 1);
				assert(sizeof(data.testImage.data[0][0].blue) == 1);

			}

			// Table
			if(data.available.tableEnable)
			{
				// Get image data
				deserializer.Read(data.table);
			}


			// Hands
			if(data.available.handsEnable)
			{
				deserializer.ReadVector(data.hands);
			}


			// Make all images that are not enabled to have zero size //
	
			// Color
			if(!data.available.colorImageEnable)
			{
				data.colorImage.rows = 0;
				data.colorImage.cols = 0;
			}

			// Depth
			if(!data.available.depthImageEnable)
			{
				data.depthImage.rows = 0;
				data.depthImage.cols = 0;
			}

			// Validity
			if(!data.available.validityImageEnable)
			{
				data.validityImage.rows = 0;
				data.validityImage.cols = 0;
			}

			// Table
			if(!data.available.testImageEnable)
			{
				data.testImage.rows = 0;
				data.testImage.cols = 0;
			}
	
			return;
		}


		private:

		ImageSerializer<ColorPixel>& colorImageSerializer;
		ImageSerializer<DepthPixel>& depthImageSerializer;
		ImageSerializer<BinaryPixel>& binaryImageSerializer;

	};

}


#endif