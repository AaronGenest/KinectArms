#ifndef KINECT_TABLE__LOCAL_CLIENT_H
#define KINECT_TABLE__LOCAL_CLIENT_H



//
// This is the KinectTable local client.
//


// Includes //


// Project
#include "DataTypes/DataTypes.h"
#include "Client.h"
#include "Sensors/Sensors.h"
#include "Network/Network.h"
#include "Threading/Threading.h"

// Standard C++


// Standard C




// Namespaces //

using namespace Threading;



namespace KinectTable
{

	// Forward class declarations
	class DataForwarder;

	
	class LocalClient : public Client
	{

	  // PUBLIC //
	  public:

		LocalClient(const SessionParameters& sessionParameters);
		LocalClient(const SessionParameters& sessionParameters, const char* localAddress);
		~LocalClient();

		//!!bool IsConnected() const;
		void GetParams(KinectDataParams& params);

		void SetParams(const KinectDataParams& params);
		void RecalculateTableCorners();
		bool GetData(KinectData& data, bool waitForNewData = false);

		void SetData(long frameNum, KinectData& data);

	  // PRIVATE //
	  private:

		// Specifies whether the session is public or private
		const SessionType sessionType;

		Sensors::DataProcessor& dataProcessor;
		
		long frameNumGiven; // Keeps track of which frame we have been given
		long frameNumTaken; // Keeps track of which frame has been taken by the user
		Mutex dataMutex;
		KinectData data;
		HANDLE newDataEvent;

		// The types of data requested by the local user
		KinectDataParams localParams;

		// Network stuff
		static const unsigned short portNum = 55555;
		Network::Server* server;

		DataForwarder* dataForwarder;
		
	};

}


#endif