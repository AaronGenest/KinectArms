#ifndef KINECT_TABLE__DATA_FORWARDER_H
#define KINECT_TABLE__DATA_FORWARDER_H


// Module Check //
//#ifndef MOD_TEMPLATE
//#error "Files outside module cannot access file directly."
//#endif


//
// This is a template.
//


// Includes //

// Module
#include "LocalClient.h"

// Project
#include "Sensors\Sensors.h"
#include "Network\Network.h"

// Standard C++


// Standard C




// Namespaces //

using namespace Sensors;
using namespace Network;


namespace KinectTable
{
	
	class DataForwarder
	{

	  // PUBLIC //
	  public:
		DataForwarder(DataProcessor& dataProcessor, LocalClient* localClient, Server* server);

	  // PRIVATE //
	  private:

		int ListeningLoop();

		DataProcessor& dataProcessor;
		LocalClient* const localClient;
		Server* const server;


		// Thread stuff
		static int ThreadStart(void* obj);
		Threading::Thread thread;
		volatile bool threadExit;

	};

}


#endif