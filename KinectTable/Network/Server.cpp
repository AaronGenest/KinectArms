//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_NETWORK

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Server.h"

// Project


// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Network
{




//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


Server::Server(const Network::Info& info, const Sockets::TcpListener::IpAddress& address, unsigned short portNum) :
	info(info),
	portNum(portNum),
	clientListener(address, portNum),
	frameNum(0)
{
	int ret = clientListener.StartListening();
	if(ret != 0)
		throw -1;

	return;
}

Server::~Server()
{
	clientListener.StopListening();
	return;
}


void Server::GetDataParams(KinectDataParams& dataParams)
{
	// Get remote clients
	static ClientListener::RemoteClientCollection remoteClients;
	clientListener.GetRemoteClients(remoteClients);

	

	// Get the data params
	ClientListener::RemoteClientCollection::iterator iterator;
	for(iterator = remoteClients.begin(); iterator != remoteClients.end(); iterator++)
	{
		RemoteClient& remoteClient = **iterator;

		// Only worry about subscribed clients
		if(remoteClient.GetSubscriptionState() != SubscriptionState::Subscribed)
			continue;

		

		static KinectDataParams remoteClientDataParams;
		remoteClient.GetKinectDataParams(remoteClientDataParams);


		// Add to data params
		dataParams.colorImageEnable |= remoteClientDataParams.colorImageEnable;
		dataParams.depthImageEnable |= remoteClientDataParams.depthImageEnable;
		dataParams.validityImageEnable |= remoteClientDataParams.validityImageEnable;
		dataParams.testImageEnable |= remoteClientDataParams.testImageEnable;

		dataParams.tableEnable |= remoteClientDataParams.tableEnable;
		dataParams.handsEnable |= remoteClientDataParams.handsEnable;

	}

	return;
}


void Server::SetData(long frameNum, KinectData& data)
{
	
	// Get remote clients
	static ClientListener::RemoteClientCollection remoteClients;
	clientListener.GetRemoteClients(remoteClients);


	bool isNewData = frameNum > this->frameNum;

	// Give data to each remote client
	ClientListener::RemoteClientCollection::iterator iterator;
	for(iterator = remoteClients.begin(); iterator != remoteClients.end(); iterator++)
	{
		RemoteClient& remoteClient = **iterator;

		// Ignore unsubscribed clients
		if(remoteClient.GetSubscriptionState() == SubscriptionState::Unsubscribed)
			continue;

		// Accept all subscription requests
		if(remoteClient.GetSubscriptionState() == SubscriptionState::Pending)
		{
			remoteClient.AcceptSubscription(info);
			continue;
		}

		// Send data if it is new
		if(isNewData)
			remoteClient.SetKinectData(data);
	}


	return;
}




//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------





}
