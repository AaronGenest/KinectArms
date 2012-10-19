#ifndef NETWORK_CONSTRUCTS_H
#define NETWORK_CONSTRUCTS_H
// Includes //
#include "TcpSocket.h"
#include "DataTypes.h"
#include "NetworkFramer.h"
//Constants//
static const size_t MaxNameLength = 10;
static const size_t IPAddressLength = 16;

//Structures//
//A Client connection to contain information pertaining to what to sent to this specific client, and how to send it.
typedef struct{
	bool subscribed;
	bool connected;
	bool hasUnsubscribed;
	Network::TcpSocket socketConnection;
	KinectDataParams dataParameters;
	char clientAddress[IPAddressLength];
	char clientName[MaxNameLength];
	NetworkFramer::Info info;
}ClientConnection;

typedef struct{
	bool subscribed;
	bool connected;
	bool framereceived;
	bool awaitingSubscription;
	Network::TcpSocket socketConnection;
	KinectDataParams dataParameters;
	char serverAddress[IPAddressLength];
	char serverName[MaxNameLength];
	NetworkFramer::Info info;
	KinectData kinectData;
}ServerConnection;

#endif