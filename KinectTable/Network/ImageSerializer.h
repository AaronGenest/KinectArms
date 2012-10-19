#ifndef NETWORK__IMAGE_SERIALIZER_H
#define NETWORK__IMAGE_SERIALIZER_H

// Module Check //
#ifndef MOD_NETWORK
#error "Files outside module cannot access file directly."
#endif


//
// These functions send and receive user requests on the network.
//


// Includes //

// Project
#include "Serializers/Serializers.h"
#include "DataTypes/DataTypes.h"
#include "Serialization/Serialization.h"


// Standard C


// Namespaces //


namespace Network
{

// NOTE: Can have a single thread serialize and image and a single thread deserialize another image at the same time using the same instance.
template<typename T>
class ImageSerializer
{

  public:
	
	void SerializeImage(typename const Types<T>::Image& image, Serialization::Serializer& serializer)
	{
	
		bool compressed;

		// Flatten image
		image.Flatten(serialFlatImage);
	
		//Compress data
		//!!Codecs::ZlibEncoder<T>::Encode(serialFlatImage, serialBytes, compressed);
		memcpy(serialBytes.data, serialFlatImage.data, serialFlatImage.size * sizeof(T));
		serialBytes.size = serialFlatImage.size * sizeof(T);
		compressed = false;

	
		// Set rows and columns
		serializer.Write(image.rows);
		serializer.Write(image.cols);
		
		// Set compressed
		serializer.Write(compressed);

		// Set image data
		serializer.Write(carraySerializer<Types<T>::Image::FlattenedArray::ByteArray::Type, Types<T>::Image::FlattenedArray::ByteArray::maxSize>(serialBytes));
		
		return;
	}



	void DeserializeImage(Serialization::Deserializer& deserializer, typename Types<T>::Image& image)
	{
	
		// Get rows and columns
		deserializer.Read(image.rows);
		deserializer.Read(image.cols);

		// Get whether the data was compressed
		bool compressed;
		deserializer.Read(compressed);

		
		// Get image data
		deserializer.Read(carraySerializer<Types<T>::Image::FlattenedArray::ByteArray::Type, Types<T>::Image::FlattenedArray::ByteArray::maxSize>(deserialBytes));


		// Decompress data
		//!!Codecs::ZlibEncoder<T>::Decode(deserialBytes, deserialFlatImage, compressed);
		memcpy(deserialFlatImage.data, deserialBytes.data, deserialBytes.size);
		assert(deserialBytes.size % sizeof(T) == 0);
		deserialFlatImage.size = deserialBytes.size / sizeof(T);

		// Get image
		image.CopyFrom(deserialFlatImage, image.rows, image.cols);

		return;
	}


  private:

	typename Types<T>::Image::FlattenedArray::ByteArray serialBytes;
	typename Types<T>::Image::FlattenedArray serialFlatImage;

	typename Types<T>::Image::FlattenedArray::ByteArray deserialBytes;
	typename Types<T>::Image::FlattenedArray deserialFlatImage;

};


}


#endif
