//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_NETWORK

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "ClientListener.h"

// Project
#include "Sockets/Sockets.h"

// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Network
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


ClientListener::ClientListener(const Sockets::TcpListener::IpAddress& address, unsigned short portNum)
	: listener(address, portNum), portNum(portNum), thread(ThreadStart), threadExit(false)
{
	
	// Start the listening thread
	thread.Start((void*)this);

	return;
}

ClientListener::~ClientListener()
{
	// Tell thread to stop
	threadExit = true;
	
	// Stop socket listener
	(void)listener.StopListening();

	// Wait for thread to stop
	(void)thread.Join();

	// Remove the clients
	RemoteClientCollection::iterator it;
	for(it = this->remoteClients.begin(); it != this->remoteClients.end(); it++)
	{
		delete &*it;
	}
	this->remoteClients.clear();


	return;
}

int ClientListener::StartListening()
{
	int ret = listener.StartListening();
	if(ret != 0)
		return -1;

	return 0;
}

int ClientListener::StopListening()
{
	int ret = listener.StopListening();
	if(ret != 0)
		return -1;

	return 0;
}


void ClientListener::GetRemoteClients(RemoteClientCollection& remoteClients)
{
	
	remoteClients.clear();

	remoteClientsMutex.Lock();
	{

		RemoteClientCollection::iterator it;

		// Remove any remote clients that are not connected anymore
		for(it = this->remoteClients.begin(); it != this->remoteClients.end(); it++)
		{
			RemoteClient& remoteClient = **it;

			// Ignore connected clients
			if(remoteClient.IsConnected())
				continue;
			
			// Remove client
			it = this->remoteClients.erase(it, it);
			delete &remoteClient;

		}


		// Add remote clients to user's list
		for(it = this->remoteClients.begin(); it != this->remoteClients.end(); it++)
		{
			// Add remote client to list
			remoteClients.push_back(*it);
		}

	}
	remoteClientsMutex.Unlock();
	
	return;
}



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


int ClientListener::ThreadStart(void* ptr)
{
	return ((ClientListener*)ptr)->ListenerLoop();
}


int ClientListener::ListenerLoop()
{
	
	// Loop until told to stop
	while(!threadExit)
	{

		// Check if we are supposed to be accepting sockets
		//!!Is a spin lock!
		if(!IsListening())
			continue;

		// Wait for connection request
		Sockets::TcpSocket* socket = listener.Accept();
		if(socket == NULL)
			continue;


		// Check if we have room for the new socket
		if(remoteClients.size() == maxRemoteClients)
			continue;

		// Create remote client
		RemoteClient* remoteClient = new RemoteClient(*socket);

		// Add socket
		remoteClientsMutex.Lock();
		{
			remoteClients.push_back(remoteClient);
		}
		remoteClientsMutex.Unlock();

		
	}

	return 0;
}


}
