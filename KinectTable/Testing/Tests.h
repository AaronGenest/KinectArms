#ifndef TESTING__TESTS_H
#define TESTING__TESTS_H

//
// This is a common place for all tests to be called.
//


// Includes //
#include "DataTypes/DataTypes.h"
#include "SocketTest.h"
#include "SocketSpeedTest.h"
#include "FramerTest.h"
#include "TableDetectionTest.h"
#include "HandDetectionTest.h"

// Project


// Standard C++


// Standard C




// Namespaces //




namespace Testing
{
	// ITests is an interface to pass Tests across the DLL boundary
	class __declspec(dllexport) ITests
	{
		// Public
		public:

		// Socket Tests
		virtual Errno RunSocketTest() = 0;
		virtual Errno RunSocketSpeedTest() = 0;

		// Framer Tests
		virtual Errno RunFramerTest() = 0;

		// Detection Tests
		virtual Errno RunTableDetectionTest(char* imagePath) = 0;
		virtual Errno RunHandDetectionTest(char* imagePath) = 0;
	};

	// Tests is a singleton to access all the tests.
	class __declspec(dllexport) Tests : public ITests
	{
	  // Public //
	  public:

		static Tests& GetTests()
		{
			static Tests instance;
			return instance;
		}

		// Socket Tests
		Errno RunSocketTest();
		Errno RunSocketSpeedTest();

		// Framer Tests
		Errno RunFramerTest();

		// Detection Tests
		Errno RunTableDetectionTest(char* imagePath);
		Errno RunHandDetectionTest(char* imagePath);

	  // Private //
	  private:

		Tests() { return; }
		Tests(const Tests&) { return; }
		void operator=(const Tests&) { return; }
	};

}


#endif
