//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Tests.h"

// Project
#include "DataTypes/DataTypes.h"
#include "Testing/SocketTest.h"
#include "Testing/SocketSpeedTest.h"
#include "Testing/FramerTest.h"
#include "Testing/TableDetectionTest.h"
#include "Testing/HandDetectionTest.h"

// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Testing
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

Errno Tests::RunSocketTest()
{
	return SocketTest::RunSocketTest();
}

Errno Tests::RunSocketSpeedTest()
{
	return SocketSpeedTest::RunSocketSpeedTest();
}

Errno Tests::RunFramerTest()
{
	return FramerTest::RunFramerTest();
}

Errno Tests::RunTableDetectionTest(char* imagePath)
{
	return TableDetectionTest::RunTableDetectionTest(imagePath);
}

Errno Tests::RunHandDetectionTest(char* imagePath)
{
	return HandDetectionTest::RunHandDetectionTest(imagePath);
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------





}
