#ifndef SERIALIZATION__DESERIALIZER_H
#define SERIALIZATION__DESERIALIZER_H


// Module Check //
#ifndef MOD_SERIALIZATION
#error "Files outside module cannot access file directly."
#endif



// Includes //

// Module
#include "ISerializable.h"

// Project

// Standard C++
#include <vector>

// Standard C
#include <assert.h>


// Namespaces //


namespace Serialization
{
	
	void Deserialize(Deserializer& deserializer, ISerializable& obj);

	class __declspec(dllexport) Deserializer
	{

	  public:

		Deserializer(const unsigned char bytes[], unsigned long length) : byteNum(0), bytes(bytes), length(length)
		{ return; }


		unsigned long BytesLeft() { return length - byteNum; }


		void Reset();
		
		void Read(bool& value)
		{
			ReadPrimitive<bool>(value);
			return;
		}
		
		
		void Read(char& value)
		{
			ReadPrimitive<char>(value);
			return;
		}

		void Read(unsigned char& value)
		{
			ReadPrimitive<unsigned char>(value);
			return;
		}

		void Read(short& value)
		{
			ReadPrimitive<short>(value);
			value = ConvertShort(value);
			return;
		}
		
		void Read(unsigned short& value)
		{
			ReadPrimitive<unsigned short>(value);
			value = ConvertShort(value);
			return;
		}

		void Read(int& value)
		{
			ReadPrimitive<int>(value);
			value = ConvertLong(value);
			return;
		}

		void Read(unsigned int& value)
		{
			ReadPrimitive<unsigned int>(value);
			value = ConvertLong(value);
			return;
		}

		void Read(long& value)
		{
			ReadPrimitive<long>(value);
			value = ConvertLong(value);
			return;
		}

		void Read(unsigned long& value)
		{
			ReadPrimitive<unsigned long>(value);
			value = ConvertLong(value);
			return;
		}

		void Read(float& value)
		{
			assert(sizeof(value) == sizeof(long));
			long& longValue = (long&)value;

			Read(longValue);
			return;
		}

		void Read(double& value)
		{
			assert(sizeof(value) == sizeof(long));
			long& longValue = (long&)value;

			Read(longValue);
			return;
		}
		
		void Read(ISerializable& obj)
		{
			obj.Deserialize(*this);
		}

		template<typename T>
		void ReadVector(std::vector<T>& elements);
		
		
		template<typename T>
		void ReadList(T* objs, unsigned long& length);


		template<>
		void ReadList<bool>(bool* objs, unsigned long& length)
		{
			assert(sizeof(bool) == sizeof(char));
			char* bytes = (char*)objs;
			ReadByteList(bytes, length);
			return;
		}

		template<>
		void ReadList<char>(char* objs, unsigned long& length)
		{
			assert(sizeof(char) == sizeof(char));
			char* bytes = (char*)objs;
			ReadByteList(bytes, length);
			return;
		}

		template<>
		void ReadList<unsigned char>(unsigned char* objs, unsigned long& length)
		{
			assert(sizeof(unsigned char) == sizeof(char));
			char* bytes = (char*)objs;
			ReadByteList(bytes, length);
			return;
		}

		template<>
		void ReadList<short>(short* objs, unsigned long& length)
		{
			assert(sizeof(short) == sizeof(short));
			short* shorts = (short*)objs;
			ReadShortList(shorts, length);
			return;
		}

		template<>
		void ReadList<unsigned short>(unsigned short* objs, unsigned long& length)
		{
			assert(sizeof(unsigned short) == sizeof(short));
			short* shorts = (short*)objs;
			ReadShortList(shorts, length);
			return;
		}

		template<>
		void ReadList<int>(int* objs, unsigned long& length)
		{
			assert(sizeof(int) == sizeof(long));
			long* longs = (long*)objs;
			ReadLongList(longs, length);
			return;
		}

		template<>
		void ReadList<unsigned int>(unsigned int* objs, unsigned long& length)
		{
			assert(sizeof(unsigned int) == sizeof(long));
			long* longs = (long*)objs;
			ReadLongList(longs, length);
			return;
		}

		template<>
		void ReadList<long>(long* objs, unsigned long& length)
		{
			assert(sizeof(long) == sizeof(long));
			long* longs = (long*)objs;
			ReadLongList(longs, length);
			return;
		}

		template<>
		void ReadList<unsigned long>(unsigned long* objs, unsigned long& length)
		{
			assert(sizeof(unsigned long) == sizeof(long));
			long* longs = (long*)objs;
			ReadLongList(longs, length);
			return;
		}

		template<>
		void ReadList<float>(float* objs, unsigned long& length)
		{
			assert(sizeof(float) == sizeof(long));
			long* longs = (long*)objs;
			ReadLongList(longs, length);
			return;
		}

		template<>
		void ReadList<double>(double* objs, unsigned long& length)
		{
			assert(sizeof(double) == sizeof(long));
			long* longs = (long*)objs;
			ReadLongList(longs, length);
			return;
		}


	  private:

		template<typename T>
		void ReadPrimitive(T& value);

		void ReadByteList(char* values, unsigned long& length);
		void ReadShortList(short* values, unsigned long& length);
		void ReadLongList(long* values, unsigned long& length);

		short ConvertShort(short value);
		long ConvertLong(long value);

		unsigned long byteNum;
		const unsigned char* const bytes;
		const unsigned long length;

	};
}


#include "Deserializer.hpp"

#endif