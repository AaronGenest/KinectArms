#pragma comment(lib, "Ws2_32.lib")
#ifndef NETWORK_TCP_CLIENT
#define NETWORK_TCP_CLIENT

//
// This class represents a socket using the TCP protocol.
//


// Includes //


#include <winsock2.h>
#include "NetworkStructures.h"
#include <time.h>
// Project


// Standard C++


// Standard C




// Namespaces //

namespace Network
{
	class NetworkClient
	{
	public:
		static NetworkClient getClient()
		{
			return networkClient;
		}
		//Call Once:
		static bool InitializeClient();
		static int ConnectToServer(const char ipAddress[IPAddressLength], unsigned short portnum, SessionType sessionType, KinectDataParams sessionParameters);
		static int SendNewDataParams(ServerConnection &server, KinectDataParams &sessionParameters);
		static int RequestInfo(ServerConnection &server, NetworkFramer::Info &info);
		static int SendUnsubscribe(ServerConnection &server);
		//Call Every main loop iteration
		static int ReceiveDataFromServer(void);//Data is placed into the kinectdata within the server structs
		static KinectData getServerData();
		static KinectDataParams getServerDataParams();
		static ServerConnection getServer();
	

	private:
		//Singleton Server
		static NetworkClient networkClient;

		NetworkFramer::Info clientInfo;

		//vector<ClientConnection> clientList;
		ServerConnection server;//Extend to send to a list of clients later


		static int HandleServerMessage(NetworkFramer::MessageType::Type messageType, ServerConnection &server);
		static int SendSubscription(ServerConnection &server, KinectDataParams &dataParams);
		NetworkClient();
		~NetworkClient();
	};

};

#endif