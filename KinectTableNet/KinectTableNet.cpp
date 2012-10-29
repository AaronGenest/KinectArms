// This is the main DLL file.

#include "Net\KinectTable.h"
#include "Net\SessionParameters.h"

using namespace KinectTableNet;

int main(array<System::String ^> ^args)
{
	KinectTableNet::SessionParameters^ params = gcnew KinectTableNet::SessionParameters();
    Console::WriteLine(KinectTableNet::KinectTable::ConnectLocal(params));
    return 0;
}
