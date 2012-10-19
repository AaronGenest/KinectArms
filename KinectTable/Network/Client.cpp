//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_NETWORK

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Client.h"

// Project
#include "Framer.h"

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

Client::Client(const char* serverAddress, unsigned short portNum) :
	socket(*Sockets::TcpSocket::Connect(serverAddress, portNum)),
	recvThread(&Client::ThreadStart),
	threadExit(false),
	subscriptionState(SubscriptionState::Unsubscribed),
	frameNum(-1),
	newDataEvent(CreateEvent(NULL, true, false, ""))
{

	// Start up receive thread
	recvThread.Start(this);
	return;
}

Client::~Client()
{

	threadExit = true;

	// Destroy socket
	delete &socket;

	// Wait for thread to finish
	recvThread.Join();

	CloseHandle(newDataEvent);

	return;
}



SubscriptionState::Type Client::GetSubscriptionState() const
{
	// Is enum atomic?
	return subscriptionState;
}


void Client::GetServerInfo(Network::Info& info)
{
	assert(subscriptionState != SubscriptionState::Unsubscribed);

	variableMutex.Lock();
	{
		memcpy(&serverInfo, &this->serverInfo, sizeof(info));
	}
	variableMutex.Unlock();

	return;
}



long Client::GetKinectData(KinectData& data, bool waitForNewData)
{
	// Check if we are subscribed
	if(subscriptionState != SubscriptionState::Subscribed)
	{
		data.available.colorImageEnable = false;
		data.available.depthImageEnable = false;
		data.available.validityImageEnable = false;
		data.available.testImageEnable = false;

		throw EBADMSG;
	}

	if(waitForNewData)
		WaitForSingleObject(newDataEvent, INFINITE);

	long frameNum;
	variableMutex.Lock();
	{
		this->recvData.CopyTo(data);
		frameNum = this->frameNum;

		ResetEvent(newDataEvent);
	}
	variableMutex.Unlock();

	return frameNum;
}



Errno Client::Subscribe(const Network::Info& info, const KinectDataParams& dataParams)
{
	// Preconditions
	assert(subscriptionState != SubscriptionState::Pending);
	assert(subscriptionState != SubscriptionState::Subscribed);

	// Create frame
	Errno ret = framer.CreateSubscribe(info, dataParams, sendFrame);
	if(ret != 0)
		return -1;

	// Send frame
	ret = socket.SendData(sendFrame.data, sendFrame.size);
	if(ret != 0)
		return -1;
	
	// Set subscription state
	subscriptionState = SubscriptionState::Pending;

	return 0;
}


Errno Client::Unsubscribe()
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

Errno Client::SetInfo(const Network::Info& info)
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



Errno Client::SetKinectDataParams(const KinectDataParams& dataParams)
{
	// Preconditions
	assert(subscriptionState != SubscriptionState::Unsubscribed);
	assert(subscriptionState != SubscriptionState::Pending);

	// Create frame
	Errno ret = framer.CreateDataParams(dataParams, sendFrame);
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


int Client::ThreadStart(void* obj)
{
	return ((Client*)obj)->RecvLoop();
}

int Client::RecvLoop()
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
			case Network::FrameType::SubscribeAck:
			{
				variableMutex.Lock();
				{
					bool accepted;
					ret = framer.RetrieveSubscribeAck(recvFrame, serverInfo, accepted);
					assert(accepted);
					
					subscriptionState = SubscriptionState::Subscribed;
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
					ret = framer.RetrieveInfo(recvFrame, serverInfo);
				}
				variableMutex.Unlock();

				assert(ret == 0);
				break;
			}

			case Network::FrameType::Data:
			{
				variableMutex.Lock();
				{
					ret = framer.RetrieveKinectData(recvFrame, recvData);
					frameNum++;

					SetEvent(newDataEvent);
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
