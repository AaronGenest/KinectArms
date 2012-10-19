#ifndef KINECT_TABLE_H
#define KINECT_TABLE_H



// Includes //
#include "DataTypes/DataTypes.h"
#include "Client.h"


// Namespaces //



namespace KinectTable
{
	
	// Methods //

	// Attempts to create a private session with a local Kinect sensor.
	// return -> Returns the connected client if successful, NULL if connection failed.
	Client* ConnectLocal(const SessionParameters& sessionParameters);
	
	
	// Attempts to create a public session with a local Kinect sensor.
	// localAddress -> The local address other computers can use to connect with the Kinect remotely.
	// return -> Returns the connected client if successful, NULL if connection failed.
	Client* ConnectLocal(const SessionParameters& sessionParameters, const char* localAddress);


	// Attempts to create a private session with a remote Kinect sensor.
	// remoteAddress -> The address of the remote Kinect.
	// return -> Returns the connected client if successful, NULL if connection failed.
	Client* ConnectRemote(const SessionParameters& sessionParameters, const char* remoteAddress);


	// Disconnects the connection with the Kinect sensor.
	void Disconnect(Client* client);

}


#endif
