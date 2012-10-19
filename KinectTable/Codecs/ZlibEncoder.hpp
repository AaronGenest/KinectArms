//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "ZlibEncoder.h"

// Project
#include "DataTypes/DataTypes.h"

// ZLib
#include "zlib.h"

// Standard C++
#include <exception>

// Standard C
#include <stdlib.h>
#include <assert.h>

//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------


namespace Codecs
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

const int compressionLevel = Z_DEFAULT_COMPRESSION;


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

template<typename T>
int ZlibEncoder<T>::Encode(typename const Types<T>::FlatImage& input, typename Types<T>::FlatImageBytes& output, bool& compressed)
{


    int ret, flush;
    z_stream strm;

    // Allocate deflate state
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, compressionLevel);
    if (ret != Z_OK)
        return ret;


	// Get input size in bytes
    size_t inputSize = input.size * sizeof(T);


	// Make sure our input data is not larger than the allowed input buffer
	sizeof(sizeof(strm.avail_in) == 4);
	const size_t maxInputSize = (size_t)0xFFFFFFFF;
	assert(inputSize <= maxInputSize);


	// Setup input
	strm.avail_in = inputSize;
    flush = Z_FINISH;
    strm.next_in = (Bytef*)input.data;

	
	// Setup output
	output.size = 0;
	strm.avail_out = output.maxSize;
	strm.next_out = (Bytef*)output.data;
	
	// Compress
	ret = deflate(&strm, flush);
	assert(ret == Z_OK || ret == Z_STREAM_END);  //state not clobbered
	assert(strm.avail_in == 0);		//all input will be used

	// Get size of compressed data
	size_t outputSize = output.maxSize - strm.avail_out;
	assert(outputSize <= output.maxSize);


	// Compare size of compressed data to original data
	// If our output size is the same or greater than the input size, then just use the input data.
	if(outputSize >= inputSize)
	{
		// Give caller original data
		memcpy(output.data, input.data, inputSize);
		output.size = inputSize;

		compressed = false;
	}
	else
	{
		output.size = outputSize;

		compressed = true;		
	}

	

    // clean up and return
    (void)deflateEnd(&strm);
    return Z_OK;
}



template<typename T>
int ZlibEncoder<T>::Decode(typename const Types<T>::FlatImageBytes& input, typename Types<T>::FlatImage& output, bool compress)
{
    int ret;
    z_stream strm;


	// Check if there is data to decompress
	if(input.size == 0)
	{
		output.size = 0;
		return Z_OK;
	}

	// Check if we are using compressed data
	if(!compress)
	{
		memcpy(output.data, input.data, input.size);
		
		assert(input.size % sizeof(T) == 0);
		output.size = input.size / sizeof(T);
		return Z_OK;
	}


    // Allocate inflate state
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;


	// Get max output size in bytes
	const size_t maxOutputSize = output.maxSize * sizeof(T);

	// Setup input
	strm.avail_in = input.size;
	strm.next_in = (Bytef*)input.data;

	// Setup output
    strm.avail_out = maxOutputSize;
    strm.next_out = (Bytef*)output.data;

	// Decompress
	ret = inflate(&strm, Z_FINISH);
	assert(ret == Z_STREAM_END); //!! || ret == Z_BUF_ERROR);
    assert(ret != Z_STREAM_ERROR);  // state not clobbered
    switch (ret)
	{
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;     // fall thru
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			(void)inflateEnd(&strm);
			return ret;
    }
    
	size_t outputSize = maxOutputSize - strm.avail_out;
	assert(outputSize % sizeof(T) == 0);
	output.size = outputSize / sizeof(T);

    // Clean up and return
    (void)inflateEnd(&strm);
    return ret == Z_OK;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------



}
