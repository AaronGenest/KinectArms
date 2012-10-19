//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "RunLengthEncoder.h"

// Project
#include "DataTypes/DataTypes.h"

// Standard C++
#include <exception>

// Standard C
#include <stdlib.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------


namespace Codecs
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


//!!We need to make sure the output does not become bigger than the input!
//!!Need to convert element's bytes to network byte order!
template<typename T>
void RunLengthEncoder::Encode(typename const Types<T>::FlatImage& input, typename Types<T>::FlatImageBytes& output)
{
	
	const int maxSequenceLength = 127;
	
	size_t outputNum = 0; //the byte num in the byte stream
	size_t seqStart = 0;  //the current sequence start
	bool seqOfSameValues = true;  //whether we are talking about a sequence of different or same numbers

	//See the frame as a 1D array of pixels
	//DepthPixel pixels[] = { 0,0,0,1,0,1,0,1,1,1,0 };
	size_t inputNum = input.size;

	//Go through each pixel
	for(size_t i=0; i<inputNum; i++)
	{

		//Get current and last pixels
		const T& current = input.data[i];
		const T& last = (i != 0) ? input.data[i-1] : current;


		//Check if we should start a new sequence
		bool sameValues = current == last;
		const char seqLengthMask = (char)0x7F;
		char seqLength = output.data[seqStart] & seqLengthMask;
		if(sameValues != seqOfSameValues || seqLength == maxSequenceLength || i == 0)
		{
			//Create a new sequence//

			//Get next pixel
			const T& next = (i < inputNum - 1) ? input.data[i+1] : current;

			//Get type of next sequence
			seqOfSameValues = current == next;
			
			//Set the start of the sequence
			seqStart = outputNum;

			//Set the sequence type according to the next two values
			const char seqTypeSame = (char)0x80;
			const char seqTypeDiff = (char)0x00;
			output.data[seqStart] = seqOfSameValues ? seqTypeSame : seqTypeDiff;
			outputNum++;

			//Add the first value of the sequence
			memcpy(&output.data[outputNum], &current, sizeof(current));
			outputNum += sizeof(current);
		}
		else
		{
			if(!seqOfSameValues)
			{
				//Add new value to sequence
				memcpy(&output.data[outputNum], &current, sizeof(current));
				outputNum += sizeof(current);
			}

		}

		output.data[seqStart]++;

	}

	//Report the length of the sequence
	output.size = outputNum;
	
	return;
}


/*
//!!We need to make sure the output does not become bigger than the input!
//!!Need to convert element's bytes to network byte order!
template<typename T>
void RunLengthEncoder<T>::Encode(const Image& input, ByteStream& output)
{
	const int maxSequenceLength = 127;

	assert(input.size <= output.maxSize);
	
	size_t outputNum = 0; //the byte num in the byte stream
	size_t seqStart = 0;  //the current sequence start
	bool seqOfSameValues = true;  //whether we are talking about a sequence of different or same numbers

	//See the frame as a 1D array of pixels
	//DepthPixel pixels[] = { 0,0,0,1,0,1,0,1,1,1,0 };

	//Go through each pixel
	for(size_t i=0; i<input.rows*input.cols; i++)
	{

		const size_t y = i / input.cols;
		const size_t x = i % input.cols;

		const size_t yLast = (i-1) / input.cols;
		const size_t xLast = (i-1) % input.cols;


		//Get current and last pixels
		const T& current = input.data[y][x];
		const T& last = (i != 0) ? input.data[yLast][xLast] : current;

		//Check if we should start a new sequence
		bool sameValues = current == last;
		const char seqLengthMask = (char)0x7F;
		char seqLength = output.data[seqStart] & seqLengthMask;
		if(sameValues != seqOfSameValues || seqLength == maxSequenceLength || i == 0)
		{
			//Create a new sequence//

			//Get next pixel
			const int yNext = (i+1) / input.cols;
			const int xNext = (i+1) % input.cols;
			const T& next = (i == (input.rows*input.cols - 1)) ? input.data[yNext][xNext] : current;

			//Get type of next sequence
			seqOfSameValues = current == next;
			
			//Set the start of the sequence
			seqStart = byteNum;

			//Set the sequence type according to the next two values
			const char seqTypeSame = (char)0x80;
			const char seqTypeDiff = (char)0x00;
			output.data[seqStart] = seqOfSameValues ? seqTypeSame : seqTypeDiff;
			outputNum++;

			//Add the first value of the sequence
			memcpy(&output.data[outputNum], &current, sizeof(current));
			outputNum += sizeof(current);
		}
		else
		{
			if(!seqOfSameValues)
			{
				//Add new value to sequence
				memcpy(&output.data[outputNum], &current, sizeof(current));
				outputNum += sizeof(current);
			}

		}

		output.data[seqStart]++;

	}

	//Report the length of the sequence
	output.size = outputNum;
	
	return;
}

*/






//!!We need to make sure the output does not become bigger than the input!
//!!Need to convert element's bytes to host byte order!
template<typename T>
void RunLengthEncoder::Decode(typename const Types<T>::FlatImageBytes& input, typename Types<T>::FlatImage& output)
{
	
	//Go through each byte of the byte stream
	size_t inputNum = 0;
	size_t outputNum = 0;
	while(inputNum < input.size)
	{
		//Get sequence type and sequence length
		const char seqTypeMask = (char)0x80;
		const char seqLengthMask = (char)0x7F;
		bool seqOfSameValues = (input.data[inputNum] & seqTypeMask) != 0;
		char seqLength = input.data[inputNum] & seqLengthMask;
		inputNum++;

		//Get pixels in the sequence
		if(seqOfSameValues)
		{
			//Copy the pixel
			for(int i=0; i<seqLength; i++)
			{
				memcpy(&output.data[outputNum], &input.data[inputNum], sizeof(T));
				outputNum++;
			}
			inputNum += sizeof(T);
		}
		else
		{
			//Copy each pixel
			for(int i=0; i<seqLength; i++)
			{
				memcpy(&output.data[outputNum], &input.data[inputNum], sizeof(T));
				outputNum++;
				inputNum += sizeof(T);
			}
		}

	}
	
	return;
}




//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------



}
