#ifndef UTIL__I_SIZABLE_H
#define UTIL__I_SIZABLE_H

//
// This is an interface for serializable objects.
//


// Includes //


// Project


// Standard C++


// Standard C




// Namespaces //




namespace Util
{
	
	class ISizable
	{

	  public:

		virtual ~ISizable() { }

		// Returns the max number of bytes this structure needs to store all its information.
		// Bytes are not necessarily adjacent to each other.
		virtual size_t SizeOf() const = 0;

	};

}


#endif
