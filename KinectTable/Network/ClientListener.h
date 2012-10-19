#ifndef NETWORK__CLIENT_LISTENER_H
#define NETWORK__CLIENT_LISTENER_H

// Module Check //
#ifndef MOD_NETWORK
#error "Files outside module cannot access file directly."
#endif


//
// This class listens to incoming requests on a certain port.
//


// Includes //


// Project
#include "DataTypes/DataTypes.h"
#include "Threading/Threading.h"
#include "Network/RemoteClient.h"
#include "Sockets/Sockets.h"

// Standard C++
#include <vector>

// Standard C




// Namespaces //




namespace Network
{
	
	class ClientListener
	{

	  public:

		typedef std::vector<RemoteClient*> RemoteClientCollection;  //!!Should change to a std::list

		// Const Fields //
		
		static const int maxRemoteClients = 10;



		// Constructor //

		ClientListener(const Sockets::TcpListener::IpAddress& address, unsigned short portNum);
		virtual ~ClientListener();


		// Methods //

		int StartListening();
		int StopListening();

		bool IsListening() const { return listener.IsListening(); }

		void GetRemoteClients(RemoteClientCollection& remoteClients);


	  private:

		// Methods //

		static int ThreadStart(void*);

		int ListenerLoop();


		// Fields //

		Sockets::TcpListener::IpAddress address;
		const unsigned short portNum;


		volatile bool threadExit;
		Threading::Thread thread;

		Sockets::TcpListener listener;

		RemoteClientCollection remoteClients;
		Threading::Mutex remoteClientsMutex;

		

	};
	
}


#endif