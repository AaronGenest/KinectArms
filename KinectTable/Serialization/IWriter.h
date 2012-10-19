#ifndef SERIALIZATION__I_WRITER_H
#define SERIALIZATION__I_WRITER_H


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
	

	//!! Note: All the virtual methods would probably slow down objects in an array (like a long char array)!
	class IWriter
	{

	  public:

		virtual void Write(char value) = 0;
		virtual void Write(unsigned char value) = 0;
		virtual void Write(short value) = 0;		
		virtual void Write(unsigned short value) = 0;
		virtual void Write(int value) = 0;
		virtual void Write(unsigned int value) = 0;
		virtual void Write(long value) = 0;
		virtual void Write(unsigned long value) = 0;
		virtual void Write(float value) = 0;
		virtual void Write(double value) = 0;
		virtual void Write(const ISerializable& obj) = 0;
		
		template<typename T>
		virtual void WriteList(T* objs, unsigned long length) = 0;
		
		template<typename T>
		virtual void WriteList(const std::vector<T> objs) = 0;
		
	};
}


#endif
