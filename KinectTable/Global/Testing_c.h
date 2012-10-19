#ifndef TESTING_C_H
#define TESTING_C_H


#ifdef WIN32
	#define DLLEXPORT __declspec(dllexport)
#else
	#define DLLEXPORT 
#endif



// Includes //
#include "Testing/Testing.h"


#ifdef __cplusplus
extern "C"
{
#endif


// Returns a class to test the KinectTable library.
DLLEXPORT Testing::ITests& GetTests();


#ifdef __cplusplus
}
#endif



#endif