#ifndef THREADING__MUTEX_H
#define THREADING__MUTEX_H

// Module Check //
#ifndef MOD_THREADING
#error "Files outside module cannot access file directly."
#endif

//
// This is a class to create a mutex.
//


// Includes //


// Project


// Standard C++


// Standard C
#include <Windows.h>



// Namespaces //




namespace Threading
{
	
	class Mutex
	{

	  public:

		Mutex();
		virtual ~Mutex();
		

		void Lock();
		int Lock(unsigned int milliseconds);

		void Unlock();


	  private:

		const HANDLE mutexHandle;
		

	};
	
}


#endif