#ifndef NETWORK__CLIENT_H
#define NETWORK__CLIENT_H


// Module Check //
#ifndef MOD_NETWORK
#error "Files outside module cannot access file directly."
#endif



//
// This is the client end of a remote connection.
//


// Includes //


// Project
#include "DataTypes/DataTypes.h"
#include "Network/Framer.h"
#include "Threading/Threading.h"
#include "Sockets/Sockets.h"

// Standard C++


// Standard C




// Namespaces //




namespace Network
{
	//!!Should keep state of requested data params
	//!!Should take data params in constructor and send it in subscribe message
	class Client
	{
		
	  public:

		Client(const char* serverAddress, unsigned short portNum);
		virtual ~Client();

		
		bool IsConnected() const { socket.IsConnected(); }

		SubscriptionState::Type GetSubscriptionState() const;
		void GetServerInfo(Network::Info& info);
		
		
		Errno Subscribe(const Network::Info& info, const KinectDataParams& dataParams);
		Errno Unsubscribe();
		Errno SetInfo(const Network::Info& info);
		Errno SetKinectDataParams(const KinectDataParams& dataParams);

		long GetKinectData(KinectData& data, bool waitForNewData = false);

	  private:
		
		static int ThreadStart(void* obj);
		int RecvLoop();

		Sockets::TcpSocket& socket;

		SubscriptionState::Type subscriptionState;
		Network::Info serverInfo;
		
		KinectData recvData;
		long frameNum;
		
		Framer::Frame sendFrame;
		Framer framer;

		Threading::Thread recvThread;
		volatile bool threadExit;

		Threading::Mutex variableMutex;

		HANDLE newDataEvent;
	};
	
}


#endif
