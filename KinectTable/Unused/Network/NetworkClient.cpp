//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "NetworkClient.h"
#include "NetworkFramer.h"
// Project


// Standard C++


// Standard C


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------


using namespace NetworkFramer;

namespace Network
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
	NetworkClient NetworkClient::networkClient;


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

	bool NetworkClient::InitializeClient()
	{
		srand((unsigned int)time(NULL));
		return true;//client is initialized and ready to start sending connection/subscription requests
	}
	int NetworkClient::ConnectToServer(const char ipAddress[IPAddressLength], unsigned short portnum, SessionType sessionType, KinectDataParams sessionParameters)
	{
		int ret = networkClient.server.socketConnection.Connect(ipAddress, portnum);
		if(ret != 0)
		{
			printf("Error: Could not connect.\n");
			return -1;
		}

		networkClient.server.connected = true;

		ret = SendSubscription(networkClient.server, sessionParameters);
		if(ret != 0)
		{
			printf("Error in subscribing to server.");
			return -1;
		}
		return 0;
	}
	int NetworkClient::ReceiveDataFromServer(void)
	{
		if(!networkClient.server.connected)
			return true;
		MessageType::Type messageType;
		int ret = 0;
		networkClient.server.framereceived = false;
		while(!networkClient.server.framereceived)
		{
			ret = NetworkFramer::ReceiveMessage(networkClient.server.socketConnection, messageType);

			if(ret != 0)
			{
				printf("Error: Could not receive data from server.\n");
				return false;
			}

			ret = networkClient.HandleServerMessage(messageType, networkClient.server);
				
			if(ret != 0)
			{
				printf("Error: Could not handle the message from server.\n");
				return false;
			}
		}
		
		return true;
	}
	int NetworkClient::SendNewDataParams(ServerConnection &server, KinectDataParams &sessionParameters)
	{
		int ret = NetworkFramer::SendDataParams(server.socketConnection, sessionParameters);
		if(ret != 0)
		{
			printf("Error: Could not send new data parameters to server.\n");
			return -1;
		}
		return 0;
	}
	int NetworkClient::RequestInfo(ServerConnection &server, NetworkFramer::Info &info)
	{
		int ret= NetworkFramer::SendInfoRequest(server.socketConnection);
		if(ret != 0)
		{
			printf("Error: couldnt send an info request to server.");
			return -1;
		}

		return 0;
	}
	int NetworkClient::SendUnsubscribe(ServerConnection &server)
	{
		int ret = NetworkFramer::SendUnsubscribe(server.socketConnection);
		if(ret != 0)
		{
			printf("Error: Could not send unsubscribe to server.\n");
			return -1;
		}
		return 0;
	}
	KinectData NetworkClient::getServerData(){return networkClient.server.kinectData;}
	KinectDataParams NetworkClient::getServerDataParams(){return networkClient.server.dataParameters;}
	ServerConnection NetworkClient::getServer(){return networkClient.server;}
//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------
	NetworkClient::NetworkClient()
	{
		
		memcpy(networkClient.clientInfo.Name(), "ClientName", sizeof(networkClient.clientInfo.Name()));
		
	}
	NetworkClient::~NetworkClient()
	{
		WSACleanup();
	}
	

	int NetworkClient::SendSubscription(ServerConnection &server, KinectDataParams &dataParams)
	{
		int	ret = NetworkFramer::SendSubscribe(server.socketConnection, networkClient.clientInfo, dataParams);
		if(ret != 0)
		{
			printf("Error: Could not send message back to client.\n");
			return -1;
		}

		MessageType::Type messageType;

		server.awaitingSubscription = true;

		while(server.awaitingSubscription){
			ret = NetworkFramer::ReceiveMessage(server.socketConnection, messageType);

			if(ret != 0)
			{
				printf("Error: Could not receive data from server.\n");
				return ret;
			}

			ret = networkClient.HandleServerMessage(messageType, server);
				
			if(ret != 0)
			{
				printf("Error: Could not handle the message from server.\n");
				return ret;
			}
		}

		return 0;
	}
	int NetworkClient::HandleServerMessage(MessageType::Type messageType, ServerConnection &server)
	{
		int ret = 0;
		bool accepted = false;
		switch(messageType)
		{
		case MessageType::Data:
			
			ret = NetworkFramer::RecvKinectData(server.dataParameters,server.kinectData);
			if(ret != 0)
			{
				printf("Error: Could not receive kinect data.\n");
				return -1;
			}
			
			server.framereceived = true;
			break;
		case MessageType::SubscribeAck:
			ret = NetworkFramer::RecvSubscribeAck(server.info, accepted);
			if(ret != 0)
			{
				printf("Error: Could not get message data.\n");
				return -1;
			}

			if(accepted)
			{
				server.subscribed = true;
			}

			server.awaitingSubscription = false;

			break;
		case MessageType::Info:

			ret = NetworkFramer::RecvInfo(server.info);
			if(ret != 0)
			{
				printf("Error: Could not get message data.\n");
				return -1;
			}

			break;
		case MessageType::Unsubscribe:

			ret = NetworkFramer::RecvUnsubscribe();
			if(ret != 0)
			{
				printf("Error: Could not get message data.\n");
				return -1;
			}
			server.awaitingSubscription = false;
			server.subscribed = false;

			break;
		case MessageType::InfoRequest:
			
			ret = NetworkFramer::RecvInfoRequest();
			if(ret != 0)
			{
				printf("Error: Could not get message data.\n");
				return ret;
			}

			ret = NetworkFramer::SendInfo(server.socketConnection,networkClient.clientInfo);
			if(ret != 0)
			{
				printf("Error: Could not send info back to client.\n");
				return ret;
			}
			break;
		
		default:
			return ret;//strange message type?
		};

		return 0;
	}
};