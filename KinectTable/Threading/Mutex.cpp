//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_THREADING


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Mutex.h"

// Project


// Standard C++


// Standard C
#include <assert.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Threading
{



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


Mutex::Mutex() : mutexHandle(CreateMutex(NULL, false, NULL))
{
	return;
}


Mutex::~Mutex()
{
	CloseHandle(mutexHandle);
	return;
}


void Mutex::Lock()
{
	int ret = Lock(INFINITE);
	assert(ret == 0);
	return;
}

int Mutex::Lock(unsigned int milliseconds)
{
	assert(sizeof(DWORD) == sizeof(unsigned int));
	
	return WaitForSingleObject(mutexHandle, milliseconds);
}

void Mutex::Unlock()
{
	(void)ReleaseMutex(mutexHandle);
	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------




}
