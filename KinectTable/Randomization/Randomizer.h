#ifndef RANDOMIZATION__RANDOMIZER_H
#define RANDOMIZATION__RANDOMIZER_H


// Module Check //
#ifndef MOD_RANDOMIZATION
#error "Files outside module cannot access file directly."
#endif


// Includes //

// Module
#include "IRandomizable.h"

// Project

// Standard C++
#include <vector>

// Standard C
#include <assert.h>

// Namespaces //


namespace Randomization
{
	

	class Randomizer
	{

	  public:

		Randomizer() : seed(0) { return; }

		Randomizer(unsigned int seed) : seed(seed)
		{
			srand(seed);
			return;
		}


		void Randomize(bool& value)
		{
			value = RandomizeByte();
			return;
		}

		void Randomize(char& value)
		{
			value = RandomizeByte();
			return;
		}

		void Randomize(unsigned char& value)
		{
			value = RandomizeByte();
			return;
		}

		void Randomize(short& value)
		{
			value = RandomizeShort();
			return;
		}
		
		void Randomize(unsigned short& value)
		{
			value = RandomizeShort();
			return;
		}

		void Randomize(int& value)
		{
			value = RandomizeLong();
			return;
		}

		void Randomize(unsigned int& value)
		{
			value = RandomizeLong();
			return;
		}

		void Randomize(long& value)
		{
			value = RandomizeLong();
			return;
		}

		void Randomize(unsigned long& value)
		{
			value = RandomizeLong();
			return;
		}

		void Randomize(float& value)
		{
			assert(sizeof(value) == sizeof(long));
			long& longValue = (long&)value;
			Randomize(longValue);
			return;
		}

		void Randomize(double value)
		{
			assert(sizeof(value) == sizeof(long));
			long& longValue = (long&)value;
			Randomize(longValue);
			return;
		}
		
		void Randomize(IRandomizable& obj)
		{
			obj.Randomize(*this);
			return;
		}

		template<typename T>
		void RandomizeVector(std::vector<T>& elements)
		{
			RandomizeList(elements.data(), elements.size());
			return;
		}



		
		template<typename T>
		void RandomizeList(T* const objs, unsigned long length)
		{
			for(int i=0; i<length; i++)
			{
				Randomize(objs[i]);
			}

			return;
		}


		template<>
		void RandomizeList<bool>(bool* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(bool));
			return;
		}

		template<>
		void RandomizeList<char>(char* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(char));
			return;
		}

		template<>
		void RandomizeList<unsigned char>(unsigned char* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(unsigned char));
			return;
		}

		template<>
		void RandomizeList<short>(short* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(short));
			return;
		}

		template<>
		void RandomizeList<unsigned short>(unsigned short* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(unsigned short));
			return;
		}

		template<>
		void RandomizeList<int>(int* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(int));
			return;
		}

		template<>
		void RandomizeList<unsigned int>(unsigned int* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(unsigned int));
			return;
		}

		template<>
		void RandomizeList<long>(long* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(long));
			return;
		}

		template<>
		void RandomizeList<unsigned long>(unsigned long* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(unsigned long));
			return;
		}

		template<>
		void RandomizeList<float>(float* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(float));
			return;
		}

		template<>
		void RandomizeList<double>(double* const objs, unsigned long length)
		{
			RandomizeByteList((char*)objs, length*sizeof(double));
			return;
		}



	  private:


		unsigned char RandomizeByte()
		{
			return rand() % 0xFF;
		}

		unsigned short RandomizeShort()
		{
			return rand() % 0xFFFF;
		}

		unsigned long RandomizeLong()
		{
			return rand();
		}

		
		void RandomizeByteList(char* values, unsigned long length)
		{

			unsigned long numInts = length / sizeof(int);
			unsigned long numBytesLeft = length % sizeof(int);

			unsigned int* ints = (unsigned int*)values;
			for(unsigned long i=0; i<numInts; i++)
			{
				Randomize(ints[i]);
			}

			for(unsigned long i=0; i<numBytesLeft; i++)
			{
				Randomize(values[length - i - 1]);
			}

			return;
		}

		
		const unsigned int seed;

	};
}



#endif
