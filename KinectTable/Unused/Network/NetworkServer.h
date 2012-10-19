#pragma comment(lib, "Ws2_32.lib")
#ifndef NETWORK_TCP_SERVER
#define NETWORK_TCP_SERVER

//
// This class represents a socket using the TCP protocol.
//


// Includes //


#include <winsock2.h>
#include "TcpSocket.h"
#include "DataTypes.h"
#include "NetworkStructures.h"
#include "NetworkFramer.h"

// Project


// Standard C++


// Standard C


// Namespaces //


namespace Network
{
	class NetworkServer{
	public:
		static NetworkServer getServer()
		{
			return networkServer;
		}
		//Call Once:
		static bool InitializeServer(const char ipAddress[IPAddressLength], unsigned short portnum, SessionType sessionType, KinectDataParams sessionParameters);

		//Call Every main loop iteration
		static bool AcceptClients(void);
		static bool SendDataToClients(KinectData &data);
		static int ReceiveClientMessages(void);
		static bool getHasUnsubscribed(void){return networkServer.client.hasUnsubscribed;}

	private:
		//Singleton Server
		static NetworkServer networkServer;

		Network::TcpSocket listeningSocket;
		KinectDataParams kinectSessionParameters;
		char serverAddress[IPAddressLength];
		
		unsigned short portNumber;
		SessionType sessionType;
		NetworkFramer::Info serverInfo;

		//vector<ClientConnection> clientList;
		ClientConnection client;//Extend to send to a list of clients later

		

		static int HandleClientMessage(NetworkFramer::MessageType::Type messageType, ClientConnection &client);
		NetworkServer();
		~NetworkServer();
		
	};
};

#endif