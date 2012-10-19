#ifndef KINECT_TABLE_C_H
#define KINECT_TABLE_C_H


#ifdef WIN32
	#define DLLEXPORT __declspec(dllexport)
#else
	#define DLLEXPORT 
#endif



// Includes //
#include "DataTypes/DataTypes.h"
#include "KinectTable/Client.h"



#ifdef __cplusplus
extern "C"
{
#endif


// Attempts to create a private session with a local Kinect sensor.
// return -> Returns the connected client if successful, NULL if connection failed.
DLLEXPORT KinectTable::Client* KinectTableConnectLocalPrivate(const DataTypes::SessionParameters* sessionParameters);
	
	
// Attempts to create a public session with a local Kinect sensor.
// localAddress -> The local address other computers can use to connect with the Kinect remotely.
// return -> Returns the connected client if successful, NULL if connection failed.
DLLEXPORT KinectTable::Client* KinectTableConnectLocalPublic(const DataTypes::SessionParameters* sessionParameters, const char* localAddress);


// Attempts to create a private session with a remote Kinect sensor.
// remoteAddress -> The address of the remote Kinect.
// return -> Returns the connected client if successful, NULL if connection failed.
DLLEXPORT KinectTable::Client* KinectTableConnectRemote(const DataTypes::SessionParameters* sessionParameters, const char* remoteAddress);

// Disconnects the connection with the Kinect sensor.
DLLEXPORT void KinectTableDisconnect(KinectTable::Client* client);

#ifdef __cplusplus
}
#endif



#endif
