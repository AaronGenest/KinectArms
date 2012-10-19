//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

//#define MOD_TEMPLATE

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "DataForwarder.h"

// Module


// Project


// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace KinectTable
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

DataForwarder::DataForwarder(DataProcessor& dataProcessor, LocalClient* localClient, Server* server)
	: dataProcessor(dataProcessor), localClient(localClient), server(server), thread(DataForwarder::ThreadStart)
{

	threadExit = false;
	thread.Start(this);

	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


int DataForwarder::ThreadStart(void* obj)
{
	return ((DataForwarder*)obj)->ListeningLoop();
}


int DataForwarder::ListeningLoop()
{
	KinectData data;
	while(!threadExit)
	{
		long frameNum = dataProcessor.GetData(data, true);
		if(localClient != NULL)
			localClient->SetData(frameNum, data);

		if(server != NULL)
			server->SetData(frameNum, data);
	}

	return 0;
}



}
