#ifndef NETWORK__SERVER_H
#define NETWORK__SERVER_H

// Module Check //
#ifndef MOD_NETWORK
#error "Files outside module cannot access file directly."
#endif


//
// This is a server to manage and communicate with clients.
//


// Includes //


// Project
#include "DataTypes/DataTypes.h"
#include "Framer.h"
#include "ClientListener.h"
#include "Sockets/Sockets.h"


// Standard C++


// Standard C




// Namespaces //



namespace Network
{
	
	class Server
	{

	  public:

		// Constructors //

		Server(const Network::Info& info, const Sockets::TcpListener::IpAddress& address, unsigned short portNum);
		virtual ~Server();

		// Returns what data params are needed
		void GetDataParams(KinectDataParams& dataParams);

		void SetData(long frameNum, KinectData& data);


	  private:

		const Network::Info info;
		const unsigned short portNum;

		long frameNum;

		ClientListener clientListener;

	};

}


#endif