#ifndef NETWORK__FRAMER_H
#define NETWORK__FRAMER_H

// Module Check //
#ifndef MOD_NETWORK
#error "Files outside module cannot access file directly."
#endif


//
// These functions send and receive user requests on the network.
//


// Includes //

// Module
#include "FrameType.h"
#include "Info.h"
#include "ImageSerializer.h"

// Project
#include "DataTypes/DataTypes.h"
#include "Sockets/Sockets.h"
#include "Serialization/Serialization.h"


// Standard C


// Namespaces //


namespace Network
{
	

	// NOTE: Only supports a single thread creating a frame and a single thread retrieving a frame at the same time using the same instance.
	// The reason why we need to create an instance of Framer is so that we can just preallocate memory once at the start.
	class Framer
	{

	  public:

		static const size_t maxFrameSize = 10000000; //!!
		typedef carray<char, maxFrameSize> Frame;
		
		
		Framer() : serializer(maxFrameSize) { return; }

		// Methods //


		//Client
		//Subscribe(Info, DataParams)
		//Unsubscribe(Reason)
		//GetServerInfo()
		//SetClientInfo(Info)
		//SetDataParams(DataParams)
		//GetSpecialData()


		//Server
		//AcceptSubscribe(Info)
		//DeclineSubscribe(Reason)
		//DropSubscribe(Reason)
		//GetClientInfo()
		//SetServerInfo(Info)
		//SetData(DataParams, Data)
		//SetSpecialData(Data)

		//Other possible messages
		//SubscriptionChanged
		//InfoChanged(Info)
	

	

		// Sends from caller //
	
		// Client
		Errno CreateSubscribe(const Info& info, const KinectDataParams& dataParams, Frame& frame);
		Errno CreateDataParams(const KinectDataParams& dataParams, Frame& frame);
		//Errno SendSpecialDataRequest(const Network::TcpSocket& socket);

		// Server
		Errno CreateSubscribeAck(const Info& info, bool acceptSubscription, Frame& frame);
		Errno CreateKinectData(const KinectDataParams& dataParams, const KinectData& data, Frame& frame);
		//Errno SendSpecialData(const Network::TcpSocket& socket, const SpecialData& data);

		// Common
		Errno CreateUnsubscribe(Frame& frame);
		Errno CreateInfoRequest(Frame& frame);
		Errno CreateInfo(const Info& info, Frame& frame);
	
	
		// Returns to caller //

		Errno GetFrameType(const Frame& frame, FrameType::Type& frameType);


		// Server
		Errno RetrieveSubscribe(const Frame& frame, Info& info, KinectDataParams& dataParams);
		Errno RetrieveDataParams(const Frame& frame, KinectDataParams& dataParams);
		//Errno RecvSpecialDataRequest(const Network::TcpSocket& socket);

		// Client
		Errno RetrieveSubscribeAck(const Frame& frame, Info& info, bool& subscriptionAccepted);
		Errno RetrieveKinectData(const Frame& frame, KinectData& data);
		//Errno RecvSpecialData(const Network::TcpSocket& socket, SpecialData& data);

		// Common
		Errno RetrieveUnsubscribe(const Frame& frame);
		Errno RetrieveInfoRequest(const Frame& frame);
		Errno RetrieveInfo(const Frame& frame, Info& info);


	  private:


		//void SerializeKinectData(const KinectDataParams& dataParams, const KinectData& data, char frameData[], unsigned long& length);
		//void DeserializeKinectData(const char frameData[], unsigned long& length, KinectData& data);

		ImageSerializer<ColorPixel> colorImageSerializer;
		ImageSerializer<DepthPixel> depthImageSerializer;
		ImageSerializer<BinaryPixel> binaryImageSerializer;


		Serialization::Serializer serializer;

	};
}


#endif
