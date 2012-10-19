#ifndef CODECS__RUN_LENGTH_ENCODER_H
#define CODECS__RUN_LENGTH_ENCODER_H

// Module Check //
#ifndef MOD_CODECS
#error "Files outside module cannot access file directly."
#endif

// Includes //
#include "DataTypes/DataTypes.h"


// Namespaces //



namespace Codecs
{

struct RunLengthEncoder
{

	template<typename T>
	static void Encode(typename const Types<T>::FlatImage& input, typename Types<T>::FlatImageBytes& output);

	template<typename T>
	static void Decode(typename const Types<T>::FlatImageBytes& input, typename Types<T>::FlatImage& output);

};

}


#include "RunLengthEncoder.hpp"

#endif
