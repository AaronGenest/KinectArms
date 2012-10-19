#ifndef CODECS__ZLIB_ENCODER_H
#define CODECS__ZLIB_ENCODER_H


// Module Check //
#ifndef MOD_CODECS
#error "Files outside module cannot access file directly."
#endif


// Includes //
#include "DataTypes/DataTypes.h"


// Namespaces //



namespace Codecs
{

template<typename T>
struct ZlibEncoder
{

	//Note: Does not convert data to network byte order.
	static int Encode(typename const Types<T>::FlatImage& input, typename Types<T>::FlatImageBytes& output, bool& compressed);

	static int Decode(typename const Types<T>::FlatImageBytes& input, typename Types<T>::FlatImage& output, bool compress);

};

}

// Template files
#include "ZlibEncoder.hpp"

#endif
