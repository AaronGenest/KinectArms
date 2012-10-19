//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_THREADING


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Thread.h"

// Project


// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Threading
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void Thread::Start()
{
	if(parameterized)
		throw -1;

	threadReturn = 0;
	this->object = NULL;
	threadHandle = CreateThread(NULL, 0, ThreadStartFunc, this, 0, &threadId);
	return;
}

void Thread::Start(void* object)
{
	if(!parameterized)
		throw -1;

	threadReturn = 0;
	this->object = object;
	threadHandle = CreateThread(NULL, 0, ThreadStartFunc, this, 0, &threadId);
	return;
}

int Thread::Join()
{
	WaitForSingleObject(threadHandle, INFINITE);
	return threadReturn;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------

DWORD WINAPI Thread::ThreadStartFunc(LPVOID param)
{
	Thread& thread = *reinterpret_cast<Thread*>(param);

	int ret;
	try
	{
		if(thread.parameterized)
			ret = thread.userParamThreadStart(thread.object);
		else
			ret = thread.userThreadStart();
	}
	catch(...)
	{
		thread.threadReturn = -1;
		return thread.threadReturn;
	}

	thread.threadReturn = ret;
	return thread.threadReturn;
}



}
