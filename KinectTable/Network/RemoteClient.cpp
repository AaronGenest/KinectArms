//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_NETWORK

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "RemoteClient.h"

// Project
#include "Framer.h"
#include "Sockets/Sockets.h"

// Standard C++


// Standard C
#include <assert.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace Network
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

RemoteClient::RemoteClient(Sockets::TcpSocket& socket) : socket(socket), recvThread(&RemoteClient::ThreadStart), threadExit(false), subscriptionState(SubscriptionState::Unsubscribed)
{

	recvThread.Start(this);
	return;
}

RemoteClient::~RemoteClient()
{
	
	threadExit = true;

	// Destroy socket
	delete &socket;

	// Wait for thread to finish
	recvThread.Join();

	return;
}


SubscriptionState::Type RemoteClient::GetSubscriptionState() const
{
	// Is enum atomic?
	return subscriptionState;
}


void RemoteClient::GetInfo(Network::Info& info)
{
	assert(subscriptionState != SubscriptionState::Unsubscribed);

	variableMutex.Lock();
	{
		memcpy(&info, &this->info, sizeof(info));
	}
	variableMutex.Unlock();

	return;
}



void RemoteClient::GetKinectDataParams(KinectDataParams& dataParams)
{
	assert(subscriptionState != SubscriptionState::Unsubscribed);

	variableMutex.Lock();
	{
		memcpy(&dataParams, &this->dataParams, sizeof(dataParams));
	}
	variableMutex.Unlock();

	return;
}



Errno RemoteClient::AcceptSubscription(const Network::Info info)
{
	// Preconditions
	assert(subscriptionState != SubscriptionState::Unsubscribed);
	assert(subscriptionState != SubscriptionState::Subscribed);

	// Create frame
	Errno ret = framer.CreateSubscribeAck(info, true, sendFrame);
	if(ret != 0)
		return -1;

	// Send frame
	ret = socket.SendData(sendFrame.data, sendFrame.size);
	if(ret != 0)
		return -1;
	
	// Set subscription state
	subscriptionState = SubscriptionState::Subscribed;

	return 0;
}

Errno RemoteClient::Unsubscribe()
{
	// Preconditions
	assert(subscriptionState != SubscriptionState::Unsubscribed);

	// Create frame
	Errno ret = framer.CreateUnsubscribe(sendFrame);
	if(ret != 0)
		return -1;

	// Send frame
	ret = socket.SendData(sendFrame.data, sendFrame.size);
	if(ret != 0)
		return -1;

	// Set subscription state
	subscriptionState = SubscriptionState::Unsubscribed;

	return 0;
}


Errno RemoteClient::SetServerInfo(const Network::Info& info)
{
	// Preconditions
	assert(subscriptionState != SubscriptionState::Unsubscribed);
	assert(subscriptionState != SubscriptionState::Pending);

	// Create frame
	Errno ret = framer.CreateInfo(info, sendFrame);
	if(ret != 0)
		return -1;

	// Send frame
	ret = socket.SendData(sendFrame.data, sendFrame.size);
	if(ret != 0)
		return -1;

	return 0;
}

Errno RemoteClient::SetKinectData(const KinectData& data)
{
	// Preconditions
	assert(subscriptionState != SubscriptionState::Unsubscribed);
	assert(subscriptionState != SubscriptionState::Pending);

	// Look at what data we can send that the client wants.
	static KinectDataParams clientDataParams;
	GetKinectDataParams(clientDataParams);
	
	clientDataParams.colorImageEnable &= dataParams.colorImageEnable;
	clientDataParams.depthImageEnable &= dataParams.depthImageEnable;
	clientDataParams.validityImageEnable &= dataParams.validityImageEnable;
	clientDataParams.testImageEnable &= dataParams.testImageEnable;

	// Create frame
	Errno ret = framer.CreateKinectData(clientDataParams, data, sendFrame);
	if(ret != 0)
		return -1;

	// Send frame
	ret = socket.SendData(sendFrame.data, sendFrame.size);
	if(ret != 0)
		return -1;

	return 0;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


int RemoteClient::ThreadStart(void* obj)
{
	return ((RemoteClient*)obj)->RecvLoop();
}

int RemoteClient::RecvLoop()
{
	
	Framer::Frame recvFrame;
	Network::FrameType::Type frameType;
	Errno ret;

	while(!threadExit)
	{
		// Get next frame
		unsigned long recvSize = recvFrame.maxSize;
		ret = socket.ReceiveData(recvFrame.data, recvSize);
		if(ret != 0)
			continue;

		// Set frame size
		recvFrame.size = recvSize;

		// Get frame type
		ret = framer.GetFrameType(recvFrame, frameType);
		if(ret != 0)
			continue;

		// Get frame data
		switch(frameType)
		{
			case Network::FrameType::Subscribe:
			{
				variableMutex.Lock();
				{
					ret = framer.RetrieveSubscribe(recvFrame, info, dataParams);
					subscriptionState = SubscriptionState::Pending;
				}
				variableMutex.Unlock();

				assert(ret == 0);
				break;
			}

			case Network::FrameType::Unsubscribe:
			{
				variableMutex.Lock();
				{
					ret = framer.RetrieveUnsubscribe(recvFrame);
					subscriptionState = SubscriptionState::Unsubscribed;
				}
				variableMutex.Unlock();

				assert(ret == 0);
				break;
			}

			case Network::FrameType::Info:
			{
				variableMutex.Lock();
				{
					ret = framer.RetrieveInfo(recvFrame, info);
				}
				variableMutex.Unlock();

				assert(ret == 0);
				break;
			}

			case Network::FrameType::DataParams:
			{
				variableMutex.Lock();
				{
					ret = framer.RetrieveDataParams(recvFrame, dataParams);
				}
				variableMutex.Unlock();

				assert(ret == 0);
				break;
			}

			default: assert(false); break;
		}
	}

	return 0;
}


}
