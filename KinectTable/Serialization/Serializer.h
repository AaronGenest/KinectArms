#ifndef SERIALIZATION__SERIALIZER_H
#define SERIALIZATION__SERIALIZER_H


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
	
	//void Serialize(Serializer& serializer, const ISerializable& obj);

	class __declspec(dllexport) Serializer
	{

	  public:

		Serializer(unsigned long capacity) : byteNum(0), bytes(capacity)
		{ return; }


		void Reset();

		unsigned long BytesLeft() { return bytes.capacity() - byteNum; }


		unsigned long GetBytes(unsigned char data[], unsigned long maxSize);


		void Write(bool value)
		{
			WritePrimitive<bool>(value);
			return;
		}

		void Write(char value)
		{
			WritePrimitive<char>(value);
			return;
		}

		void Write(unsigned char value)
		{
			WritePrimitive<unsigned char>(value);
			return;
		}

		void Write(short value)
		{
			WritePrimitive<short>(ConvertShort(value));
			return;
		}
		
		void Write(unsigned short value)
		{
			WritePrimitive<unsigned short>(ConvertShort(value));
			return;
		}

		void Write(int value)
		{
			WritePrimitive<int>(ConvertLong(value));
			return;
		}

		void Write(unsigned int value)
		{
			WritePrimitive<unsigned int>(ConvertLong(value));
			return;
		}

		void Write(long value)
		{
			WritePrimitive<long>(ConvertLong(value));
			return;
		}

		void Write(unsigned long value)
		{
			WritePrimitive<unsigned long>(ConvertLong(value));
			return;
		}

		void Write(float value)
		{
			assert(sizeof(value) == sizeof(long));
			long& longValue = (long&)value;
			Write(longValue);
			return;
		}

		void Write(double value)
		{
			assert(sizeof(value) == sizeof(long));
			long& longValue = (long&)value;
			Write(longValue);
			return;
		}
		
		void Write(const ISerializable& obj)
		{
			obj.Serialize(*this);
			return;
		}

		template<typename T>
		void WriteVector(const std::vector<T>& elements)
		{
			// Make sure T implements the ISerializable interface
			//dynamic_cast<ISerializable*>(&T());
	
			WriteList(elements.data(), elements.size());
			return;
		}

		
		/*
		template<typename T>
		void Write(const T& obj)
		{
			Serialize(*this, obj);
		}
		*/

		/*
		template<>
		void Write<ISerializable>(const ISerializable& obj)
		{
			obj.Serialize(*this);
			return;
		}
		*/
		template<typename T>
		void WriteList(const T* objs, unsigned long length);


		template<>
		void WriteList<bool>(const bool* objs, unsigned long length)
		{
			assert(sizeof(bool) == sizeof(char));
			const char* bytes = (const char*)objs;
			WriteByteList(bytes, length);
			return;
		}

		template<>
		void WriteList<char>(const char* objs, unsigned long length)
		{
			assert(sizeof(char) == sizeof(char));
			const char* bytes = (const char*)objs;
			WriteByteList(bytes, length);
			return;
		}

		template<>
		void WriteList<unsigned char>(const unsigned char* objs, unsigned long length)
		{
			assert(sizeof(unsigned char) == sizeof(char));
			const char* bytes = (const char*)objs;
			WriteByteList(bytes, length);
			return;
		}

		template<>
		void WriteList<short>(const short* objs, unsigned long length)
		{
			assert(sizeof(short) == sizeof(short));
			const short* shorts = (const short*)objs;
			WriteShortList(shorts, length);
			return;
		}

		template<>
		void WriteList<unsigned short>(const unsigned short* objs, unsigned long length)
		{
			assert(sizeof(unsigned short) == sizeof(short));
			const short* shorts = (const short*)objs;
			WriteShortList(shorts, length);
			return;
		}

		template<>
		void WriteList<int>(const int* objs, unsigned long length)
		{
			assert(sizeof(int) == sizeof(long));
			const long* longs = (const long*)objs;
			WriteLongList(longs, length);
			return;
		}

		template<>
		void WriteList<unsigned int>(const unsigned int* objs, unsigned long length)
		{
			assert(sizeof(unsigned int) == sizeof(long));
			const long* longs = (const long*)objs;
			WriteLongList(longs, length);
			return;
		}

		template<>
		void WriteList<long>(const long* objs, unsigned long length)
		{
			assert(sizeof(long) == sizeof(long));
			const long* longs = (const long*)objs;
			WriteLongList(longs, length);
			return;
		}

		template<>
		void WriteList<unsigned long>(const unsigned long* objs, unsigned long length)
		{
			assert(sizeof(unsigned long) == sizeof(long));
			const long* longs = (const long*)objs;
			WriteLongList(longs, length);
			return;
		}

		template<>
		void WriteList<float>(const float* objs, unsigned long length)
		{
			assert(sizeof(float) == sizeof(long));
			const long* longs = (const long*)objs;
			WriteLongList(longs, length);
			return;
		}

		template<>
		void WriteList<double>(const double* objs, unsigned long length)
		{
			assert(sizeof(double) == sizeof(long));
			const long* longs = (const long*)objs;
			WriteLongList(longs, length);
			return;
		}

		/*
		template<typename T>
		void WriteList(const std::vector<T> objs)
		{
			this->WritePrimitive<unsigned long>(objs.size());
			for(int i=0; i<objs.size(); i++)
			{
				this->Write(objs[0]);
			}

			return;
		}
		*/


	  private:

		template<typename T>
		void WritePrimitive(T value);

		
		void WriteByteList(const char* values, unsigned long length);
		void WriteShortList(const short* values, unsigned long length);
		void WriteLongList(const long* values, unsigned long length);

		short ConvertShort(short value);
		long ConvertLong(long value);

		unsigned long byteNum;
		std::vector<unsigned char> bytes;

	};
}

#include "Serializer.hpp"

#endif
