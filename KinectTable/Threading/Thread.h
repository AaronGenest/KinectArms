#ifndef THREADING__THREAD_H
#define THREADING__THREAD_H

// Module Check //
#ifndef MOD_THREADING
#error "Files outside module cannot access file directly."
#endif


//
// This is a class to create threads.
//


// Includes //


// Project


// Standard C++


// Standard C
#include <Windows.h>



// Namespaces //




namespace Threading
{
	
	class Thread
	{

	  public:

		typedef int (*ThreadStart)(void);
		typedef int (*ParameterizedThreadStart)(void*);

		Thread(ThreadStart threadStart_) : userThreadStart(threadStart_), userParamThreadStart(NULL), parameterized(false)
		{ return; }

		Thread(ParameterizedThreadStart threadStart_) : userThreadStart(NULL), userParamThreadStart(threadStart_), parameterized(true)
		{ return; }


		void Start();
		void Start(void* object);
		
		int Join();


	  private:

		static DWORD WINAPI ThreadStartFunc(LPVOID lpParam);
		
		const bool parameterized;
		const ThreadStart userThreadStart;
		const ParameterizedThreadStart userParamThreadStart;
		
		DWORD threadId;
		HANDLE threadHandle;

		void* object;

		int threadReturn;
	};
	
}


#endif