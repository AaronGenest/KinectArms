//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "NetworkServer.h"
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
	NetworkServer NetworkServer::networkServer;


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

	bool NetworkServer::InitializeServer(const char ipAddress[IPAddressLength], unsigned short portnum, SessionType sessionType, KinectDataParams sessionParameters)
	{
		memcpy(networkServer.serverAddress, ipAddress, 16);
		memcpy(networkServer.serverInfo.Name(), "ServerName", sizeof(networkServer.serverInfo.Name()));
		networkServer.sessionType = sessionType;
		networkServer.kinectSessionParameters = sessionParameters;
		networkServer.portNumber = portnum;

		printf("Binding socket...\n");
		printf("\tAddress: %s\n", ipAddress);
		printf("\tPort Num: %hu\n", portnum);
		int ret = networkServer.listeningSocket.Bind(networkServer.serverAddress, networkServer.portNumber);
		if(ret != 0)
		{
			printf("Error: Could not bind.\n");
			return false;
		}
		printf("Socket is bound.\n\n");

		// Listen
		printf("Attempt to Listen on socket...\n");
		ret = networkServer.listeningSocket.Listen();
		if(ret != 0)
		{
			printf("Error: Could not listen.\n");
			return false;;
		}
		printf("Listening on socket.\n\n");

		return true;//Server is initialized and ready to accept connections in a main loop as well as send kinect data to subscribers
	}
	bool NetworkServer::AcceptClients(void)
	{
		static bool clientconnected = false;
		//Call a asynchronous accept, if something connects add it to a clientlist?
		int ret = networkServer.listeningSocket.Accept(networkServer.client.socketConnection, true);

		

		if(ret != 0)
		{	
			printf("Error: Couldnt accept a connection");
			return false;
		}

		if(!clientconnected && networkServer.client.socketConnection.IsConnected())
		{
			//something connected! create a client for it and add to list(later)
			//networkServer.client.socketConnection = socket;
			clientconnected = true;
			networkServer.client.connected = true;
			networkServer.client.hasUnsubscribed = false;
			networkServer.client.socketConnection.SetAsynchronous(false);
			printf("client connected!\n");
		}

		return true;
	}
	bool NetworkServer::SendDataToClients(KinectData &data)
	{
		//Send out your kinectdata to all clients based on the sessions parameters and the clients data parameters
		if(!networkServer.client.connected || !networkServer.client.subscribed)
			return true;//no errors

		KinectDataParams dataParams;
		dataParams.colorImageEnable =  networkServer.kinectSessionParameters.colorImageEnable & networkServer.client.dataParameters.colorImageEnable;
		dataParams.depthImageEnable =  networkServer.kinectSessionParameters.depthImageEnable & networkServer.client.dataParameters.depthImageEnable;
		dataParams.tableImageEnable =  networkServer.kinectSessionParameters.tableImageEnable & networkServer.client.dataParameters.tableImageEnable;
		dataParams.validityImageEnable =  networkServer.kinectSessionParameters.validityImageEnable & networkServer.client.dataParameters.validityImageEnable;

		//Remove some kinect data based on these parameters to reduce the amount of data sent? LOOK AT LATER

		int ret = 0;
		ret = NetworkFramer::SendKinectData(networkServer.client.socketConnection, dataParams, data);
		if(ret != 0)
		{
			printf("Error: Could not send message back to client.\n");
			return false;
		}

		return true;
	}
	int NetworkServer::ReceiveClientMessages(void)
	{
		//Receive messages from all clients and act accordingly
		if(!networkServer.client.connected)
			return 0;//return false for errors

		MessageType::Type messageType;
		int ret = NetworkFramer::ReceiveMessage(networkServer.client.socketConnection, messageType);
		if(ret != 0)
		{
			int nError = WSAGetLastError();
			if(nError == WSAEWOULDBLOCK)
			{
				//would block due to accept being asynchonrous
				printf("asdf");
				return 0;
			}
			printf("Error: %d\nCould not receive data.\n", nError);
			return -1;
		}

		ret = HandleClientMessage(messageType, networkServer.client);

		return ret;
	}
	
//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------
	NetworkServer::NetworkServer()
	{
		
	}
	NetworkServer::~NetworkServer()
	{
		WSACleanup();
	}
	int NetworkServer::HandleClientMessage(MessageType::Type messageType, ClientConnection &client)
	{
		int ret = 0;
		bool accepted = true;
		switch(messageType)
		{
			case MessageType::Subscribe:
				//setup the client to be subscribe and reply
				ret = NetworkFramer::RecvSubscribe(client.info, client.dataParameters);

				//Check if you want to allow all connectinons or filter them later, for now accept all subscriptions 
				client.subscribed = true;

				if(ret != 0){
					printf("Error: Could not get message data.\n");
					return ret;
				}

				ret = NetworkFramer::SendSubscribeAck(client.socketConnection, networkServer.serverInfo, accepted);
				if(ret != 0)
				{
					printf("Error: Could not get send subscription ack.\n");
					return ret;
				}
				break;
			case MessageType::DataParams:
				//Change the clients data params

				ret = NetworkFramer::RecvDataParams(client.dataParameters);
				if(ret!=0)
				{
					printf("Error: Could not get clients data parameters.\n");
					return ret;
				}
				break;
			case MessageType::InfoRequest:
				//reply to info request
				
				ret = NetworkFramer::RecvInfoRequest();
				if(ret != 0)
				{
					printf("Error: Could not get message data.\n");
					return ret;
				}

				ret = NetworkFramer::SendInfo(client.socketConnection,networkServer.serverInfo);
				if(ret != 0)
				{
					printf("Error: Could not send info back to client.\n");
					return ret;
				}
				break;
			case MessageType::Unsubscribe:
				//unsubscribe this client

				ret = NetworkFramer::RecvUnsubscribe();
				if(ret != 0)
				{
					printf("Error: Could not get message data.\n");
					return ret;
				}

				client.subscribed = false;
				client.hasUnsubscribed = true;
				break;
			case MessageType::Info:
				//received info from this client
				ret = NetworkFramer::RecvInfo(client.info);
				if(ret != 0)
				{
					printf("Error: Could not get client info data.\n");
					return ret;
				}

				break;
			default:
				return ret;//strange message type?
		};

		return 0;
	}
};