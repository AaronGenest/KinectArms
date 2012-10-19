#ifndef KINECT_TABLE__REMOTE_CLIENT_H
#define KINECT_TABLE__REMOTE_CLIENT_H



//
// This is the KinectTable local client.
//


// Includes //


// Project
#include "DataTypes/DataTypes.h"
#include "Client.h"
#include "Network/Network.h"

// Standard C++


// Standard C




// Namespaces //




namespace KinectTable
{
	
	//!! Remote client could also host public 
	class RemoteClient : public Client
	{

	  // PUBLIC //
	  public:

		RemoteClient(const SessionParameters& sessionParameters, const char* remoteAddress);
		~RemoteClient();

		//!!bool IsConnected() const;
		void GetParams(KinectDataParams& params);

		void SetParams(const KinectDataParams& params);
		void RecalculateTableCorners();
		bool GetData(KinectData& data, bool waitForNewData = false);

	  // PRIVATE //
	  private:

		// The types of data requested by the local user
		KinectDataParams localParams;

		Network::Client* client;
		long frameNum; // Keeps track of which frame we had last
		
		// Network stuff
		static const unsigned short portNum = 55555;
		
	};

}


#endif