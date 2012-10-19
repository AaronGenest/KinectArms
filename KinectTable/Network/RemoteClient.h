#ifndef NETWORK__REMOTE_CLIENT_H
#define NETWORK__REMOTE_CLIENT_H

// Module Check //
#ifndef MOD_NETWORK
#error "Files outside module cannot access file directly."
#endif


//
// This is a virtualization of the client on the other end of a remote connection.
//


// Includes //


// Project
#include "DataTypes/KinectDataParams.h"
#include "Network/Framer.h"
#include "Threading/Threading.h"
#include "Sockets/Sockets.h"

// Standard C++


// Standard C




// Namespaces //




namespace Network
{
	//!! Would be nice if it could do stuff like accept subscription on its own.
	class RemoteClient
	{
		
	  public:

		RemoteClient(Sockets::TcpSocket& socket);
		virtual ~RemoteClient();


		bool IsConnected() const { return socket.IsConnected(); }


		SubscriptionState::Type GetSubscriptionState() const;
		void GetInfo(Network::Info& info);
		void GetKinectDataParams(DataTypes::KinectDataParams& dataParams);


		Errno AcceptSubscription(const Network::Info info);
		//!!Deny subscription()?
		Errno Unsubscribe();
		Errno SetServerInfo(const Network::Info& info);
		Errno SetKinectData(const KinectData& data);
		

	  private:

		static int ThreadStart(void* obj);
		int RecvLoop();

		Sockets::TcpSocket& socket;

		SubscriptionState::Type subscriptionState;
		Network::Info info;
		KinectDataParams dataParams;
		Framer::Frame sendFrame;
		Framer framer;

		Threading::Mutex variableMutex;

		Threading::Thread recvThread;
		volatile bool threadExit;
	};
	
}


#endif