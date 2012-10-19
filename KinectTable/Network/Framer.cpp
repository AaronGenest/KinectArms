//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_NETWORK

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Framer.h"

// Module
#include "KinectDataSerializer.h"

// Project
#include "Codecs/Codecs.h"
#include "Serialization/Serialization.h"

// Standard C
#include <assert.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace Serialization;


//NOTE: We are using multiple RetrieveXXX() functions instead of giving memory as a byte array and having the user cast it to the proper struct.
//		We are doing this because we can now do error checking to make sure the user asks for the right data type, and now all the structs
//			do not have to be serializable (tightly packed into an array).
namespace Network
{




struct FrameInterface
{

  public:
	
	typedef unsigned char DataArray[Framer::maxFrameSize - sizeof(FrameType::Type)];

	const FrameType::Type& FrameType() const { return (FrameType::Type&)bytes[0]; }
	const DataArray& FrameData() const { return (DataArray&)bytes[1]; }

	FrameType::Type& FrameType() { return (FrameType::Type&)bytes[0]; }
	DataArray& FrameData() { return (DataArray&)bytes[1]; }
	
  protected:

	FrameInterface()
	{
		// Make sure frame type is only one byte
		assert(sizeof(FrameType::Type) == 1);  //!!Won't actually get run
		return;
	}


  private:

	char bytes[Framer::maxFrameSize];

};






//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------



// Sends from caller //
	
// Client


Errno Framer::CreateSubscribe(const Info& info, const KinectDataParams& dataParams, Frame& frame)
{
	// Get frame interface
	FrameInterface& frameInterface = (FrameInterface&)frame.data;

	// Initialize variables
	unsigned long dataLength = sizeof(frameInterface.FrameData());
	
	// Set frame type
	frameInterface.FrameType() = FrameType::Subscribe;

	// Set frame data
	FrameInterface::DataArray& frameData = frameInterface.FrameData();

	serializer.Reset();
	serializer.Write(info);
	serializer.Write(dataParams);
	dataLength = serializer.GetBytes(frameData, dataLength);

	frame.size = sizeof(FrameType::Type) + dataLength;
	return 0;
}

Errno Framer::CreateDataParams(const KinectDataParams& dataParams, Frame& frame)
{
	// Get frame interface
	FrameInterface& frameInterface = (FrameInterface&)frame.data;

	// Initialize variables
	unsigned long dataLength = sizeof(frameInterface.FrameData());
	
	// Set frame type
	frameInterface.FrameType() = FrameType::DataParams;

	// Set frame data
	FrameInterface::DataArray& frameData = frameInterface.FrameData();

	serializer.Reset();
	serializer.Write(dataParams);
	dataLength = serializer.GetBytes(frameData, dataLength);

	frame.size = sizeof(FrameType::Type) + dataLength;
	return 0;
}



// Server

Errno Framer::CreateSubscribeAck(const Info& info, bool acceptSubscription, Frame& frame)
{
	// Get frame interface
	FrameInterface& frameInterface = (FrameInterface&)frame.data;

	// Initialize variables
	unsigned long dataLength = sizeof(frameInterface.FrameData());
	
	// Set frame type
	frameInterface.FrameType() = FrameType::SubscribeAck;

	// Set frame data
	FrameInterface::DataArray& frameData = frameInterface.FrameData();

	serializer.Reset();
	serializer.Write(info);
	serializer.Write(acceptSubscription);
	dataLength = serializer.GetBytes(frameData, dataLength);

	frame.size = sizeof(FrameType::Type) + dataLength;
	return 0;
}

Errno Framer::CreateKinectData(const KinectDataParams& dataParams, const KinectData& data, Frame& frame)
{
	// Get frame interface
	FrameInterface& frameInterface = (FrameInterface&)frame.data;

	// Initialize variables
	unsigned long dataLength = sizeof(frameInterface.FrameData());
	
	// Set frame type
	frameInterface.FrameType() = FrameType::Data;

	
	// Set frame data
	FrameInterface::DataArray& frameData = frameInterface.FrameData();


	// Setup frame
	serializer.Reset();
	serializer.Write(dataParams);
	serializer.Write(KinectDataSerializer(const_cast<KinectData&>(data), colorImageSerializer, depthImageSerializer, binaryImageSerializer));
	dataLength = serializer.GetBytes(frameData, dataLength);

	frame.size = sizeof(FrameType::Type) + dataLength;
	return 0;
}



// Common

Errno Framer::CreateUnsubscribe(Frame& frame)
{
	// Get frame interface
	FrameInterface& frameInterface = (FrameInterface&)frame.data;

	// Set frame type
	frameInterface.FrameType() = FrameType::Unsubscribe;

	frame.size = sizeof(FrameType::Type);
	return 0;
}



Errno Framer::CreateInfoRequest(Frame& frame)
{
	// Get frame interface
	FrameInterface& frameInterface = (FrameInterface&)frame.data;

	// Set frame type
	frameInterface.FrameType() = FrameType::InfoRequest;

	frame.size = sizeof(FrameType::Type);
	return 0;
}


Errno Framer::CreateInfo(const Info& info, Frame& frame)
{
	// Get frame interface
	FrameInterface& frameInterface = (FrameInterface&)frame.data;

	// Initialize variables
	unsigned long dataLength = sizeof(frameInterface.FrameData());

	// Set frame type
	frameInterface.FrameType() = FrameType::Info;

	// Set frame data
	FrameInterface::DataArray& frameData = frameInterface.FrameData();

	// Setup frame
	serializer.Reset();
	serializer.Write(info);
	dataLength = serializer.GetBytes(frameData, dataLength);

	frame.size = sizeof(FrameType::Type) + dataLength;
	return 0;
}



Errno Framer::GetFrameType(const Frame& frame, FrameType::Type& frameType)
{
	// Get frame interface
	const FrameInterface& frameInterface = (const FrameInterface&)frame.data;

	// Get frame type
	frameType = frameInterface.FrameType();

	return 0;
}




// Server


Errno Framer::RetrieveSubscribe(const Frame& frame, Info& info, KinectDataParams& dataParams)
{
	// Get frame interface
	const FrameInterface& frameInterface = (const FrameInterface&)frame.data;

	// Make sure we have proper frame type
	if(frameInterface.FrameType() != FrameType::Subscribe)
		return -1;

	// Initialize variables
	const FrameInterface::DataArray& frameData = (const FrameInterface::DataArray&)frameInterface.FrameData();
	const unsigned long dataLength = frame.size - sizeof(FrameType::Type);

	// Get frame data
	Serialization::Deserializer deserializer(frameData, dataLength);
	deserializer.Read(info);
	deserializer.Read(dataParams);

	return 0;
}


Errno Framer::RetrieveDataParams(const Frame& frame, KinectDataParams& dataParams)
{
	// Get frame interface
	const FrameInterface& frameInterface = (const FrameInterface&)frame.data;

	// Make sure we have proper frame type
	if(frameInterface.FrameType() != FrameType::DataParams)
		return -1;

	// Initialize variables
	const FrameInterface::DataArray& frameData = (const FrameInterface::DataArray&)frameInterface.FrameData();
	const unsigned long dataLength = frame.size - sizeof(FrameType::Type);
	
	// Get frame data
	Serialization::Deserializer deserializer(frameData, dataLength);
	deserializer.Read(dataParams);
	
	return 0;
}




// Client

Errno Framer::RetrieveSubscribeAck(const Frame& frame, Info& info, bool& subscriptionAccepted)
{
	// Get frame interface
	const FrameInterface& frameInterface = (const FrameInterface&)frame.data;

	// Make sure we have proper frame type
	if(frameInterface.FrameType() != FrameType::SubscribeAck)
		return -1;

	// Initialize variables
	const FrameInterface::DataArray& frameData = (const FrameInterface::DataArray&)frameInterface.FrameData();
	const unsigned long dataLength = frame.size - sizeof(FrameType::Type);

	// Get frame data
	Serialization::Deserializer deserializer(frameData, dataLength);
	deserializer.Read(info);
	deserializer.Read(subscriptionAccepted);

	return 0;
}


Errno Framer::RetrieveKinectData(const Frame& frame, KinectData& data)
{
	// Get frame interface
	const FrameInterface& frameInterface = (const FrameInterface&)frame.data;

	// Make sure we have proper frame type
	if(frameInterface.FrameType() != FrameType::Data)
		return -1;

	// Initialize variables
	const FrameInterface::DataArray& frameData = (const FrameInterface::DataArray&)frameInterface.FrameData();
	const unsigned long dataLength = frame.size - sizeof(FrameType::Type);

	// Get frame data
	KinectDataParams dataParams;
	Serialization::Deserializer deserializer(frameData, dataLength);
	deserializer.Read(dataParams);
	deserializer.Read(KinectDataSerializer(data, colorImageSerializer, depthImageSerializer, binaryImageSerializer));

	data.available = dataParams;

	return 0;
}




// Common


Errno Framer::RetrieveUnsubscribe(const Frame& frame)
{
	// Get frame interface
	const FrameInterface& frameInterface = (const FrameInterface&)frame.data;

	// Make sure we have proper frame type
	if(frameInterface.FrameType() != FrameType::Unsubscribe)
		return -1;

	return 0;
}


Errno Framer::RetrieveInfoRequest(const Frame& frame)
{
	// Get frame interface
	const FrameInterface& frameInterface = (const FrameInterface&)frame.data;

	// Make sure we have proper frame type
	if(frameInterface.FrameType() != FrameType::InfoRequest)
		return -1;

	return 0;
}


Errno Framer::RetrieveInfo(const Frame& frame, Info& info)
{
	// Get frame interface
	const FrameInterface& frameInterface = (const FrameInterface&)frame.data;

	// Make sure we have proper frame type
	if(frameInterface.FrameType() != FrameType::Info)
		return -1;


	// Initialize variables
	const FrameInterface::DataArray& frameData = (const FrameInterface::DataArray&)frameInterface.FrameData();
	const unsigned long dataLength = frame.size - sizeof(FrameType::Type);
	
	// Get frame data
	Serialization::Deserializer deserializer(frameData, dataLength);
	deserializer.Read(info);

	return 0;
}






//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------

/*
void Framer::SerializeKinectData(const KinectDataParams& dataParams, const KinectData& data, char frameData[], unsigned long& frameDataLength)
{

	// Initialize variables
	unsigned long byteNum = 0;

	// Set data parameters in frame //
	{
		unsigned long length = frameDataLength - byteNum;
		data.available.Serialize(&frameData[byteNum], length);
		byteNum += length;
	}


	// Set images in frame //

	// Color Image
	if(data.available.colorImageEnable)
	{

		// Convert values to network byte order
		assert(sizeof(data.colorImage.data[0][0].red) == 1);
		assert(sizeof(data.colorImage.data[0][0].green) == 1);
		assert(sizeof(data.colorImage.data[0][0].blue) == 1);

		// Set image data
		unsigned long length = frameDataLength - byteNum;
		colorImageSerializer.SerializeImage(data.colorImage, &frameData[byteNum], length);
		byteNum += length;
	}



	// Depth Image
	if(data.available.depthImageEnable)
	{

		// Convert values to network byte order
		//!! Shouldn't be touching user's data!
		assert(data.depthImage.typeSize == sizeof(short));
		for(size_t y=0; y<data.depthImage.rows; y++)
		{
			for(size_t x=0; x<data.depthImage.cols; x++)
			{
				data.depthImage.data[y][x] = htons(data.depthImage.data[y][x]);
			}
		}

		// Set image data
		unsigned long length = frameDataLength - byteNum;
		depthImageSerializer.SerializeImage(data.depthImage, &frameData[byteNum], length);
		byteNum += length;

		// Convert user's values back to host byte order
		assert(data.depthImage.typeSize == sizeof(short));
		for(size_t y=0; y<data.depthImage.rows; y++)
		{
			for(size_t x=0; x<data.depthImage.cols; x++)
			{
				data.depthImage.data[y][x] = ntohs(data.depthImage.data[y][x]);
			}
		}
	}



	// Validity Image
	if(data.available.validityImageEnable)
	{
		// Convert values to network byte order
		assert(data.validityImage.typeSize == 1);
		
		// Set image data
		unsigned long length = frameDataLength - byteNum;
		binaryImageSerializer.SerializeImage(data.validityImage, &frameData[byteNum], length);
		byteNum += length;
	}



	// Test Image
	if(data.available.testImageEnable)
	{
		// Convert values to network byte order
		assert(data.testImage.typeSize == 1);

		// Set image data
		unsigned long length = frameDataLength - byteNum;
		colorImageSerializer.SerializeImage(data.testImage, &frameData[byteNum], length);
		byteNum += length;
	}
	

	// Table
	if(data.available.tableEnable)
	{
		unsigned long length = frameDataLength - byteNum;
		data.table.Serialize(&frameData[byteNum], length);
		byteNum += length;
	}

	// Hands
	if(data.available.handsEnable)
	{
		unsigned long length = frameDataLength - byteNum;
		Util::Serializer_<Hand>::Serialize(data.hands, &frameData[byteNum], length);
		byteNum += length;
	}


	frameDataLength = byteNum;
	return;
}



void Framer::DeserializeKinectData(const char frameData[], unsigned long& frameDataLength, KinectData& data)
{

	// Initialize variables
	unsigned long byteNum = 0;
	

	// Get data parameters //
	{
		unsigned long length = frameDataLength - byteNum;
		data.available.Deserialize(&frameData[byteNum], length);
		byteNum += length;
	}


	// Get images //

	// Color Image
	if(data.available.colorImageEnable)
	{

		// Get image data
		unsigned long length = frameDataLength - byteNum;
		colorImageSerializer.DeserializeImage(&frameData[byteNum], length, data.colorImage);
		byteNum += length;

		// Convert values to network byte order
		assert(sizeof(data.colorImage.data[0][0].red) == 1);
		assert(sizeof(data.colorImage.data[0][0].green) == 1);
		assert(sizeof(data.colorImage.data[0][0].blue) == 1);

	}



	// Depth Image
	if(data.available.depthImageEnable)
	{
		// Get image data
		unsigned long length = frameDataLength - byteNum;
		depthImageSerializer.DeserializeImage(&frameData[byteNum], length, data.depthImage);
		byteNum += length;

		// Convert values to network byte order
		assert(data.depthImage.typeSize == sizeof(short));
		for(size_t y=0; y<data.depthImage.rows; y++)
		{
			for(size_t x=0; x<data.depthImage.cols; x++)
			{
				data.depthImage.data[y][x] = ntohs(data.depthImage.data[y][x]);
			}
		}

	}

	// Validity Image
	if(data.available.validityImageEnable)
	{
		// Get image data
		unsigned long length = frameDataLength - byteNum;
		binaryImageSerializer.DeserializeImage(&frameData[byteNum], length, data.validityImage);
		byteNum += length;

		// Convert values to network byte order
		assert(data.validityImage.typeSize == 1);
	}

	// Test Image
	if(data.available.testImageEnable)
	{
		// Get image data
		unsigned long length = frameDataLength - byteNum;
		colorImageSerializer.DeserializeImage(&frameData[byteNum], length, data.testImage);
		byteNum += length;

		// Convert values to network byte order
		assert(data.testImage.typeSize == 1);

	}

	// Table
	if(data.available.tableEnable)
	{
		// Get image data
		unsigned long length = frameDataLength - byteNum;
		data.table.Deserialize(&frameData[byteNum], length);
		byteNum += length;
	}


	// Hands
	if(data.available.handsEnable)
	{
		// Get image data
		unsigned long length = frameDataLength - byteNum;
		Util::Serializer_<Hand>::Deserialize(&frameData[byteNum], length, data.hands);
		byteNum += length;
	}


	// Make all images that are not enabled to have zero size //
	
	// Color
	if(!data.available.colorImageEnable)
	{
		data.colorImage.rows = 0;
		data.colorImage.cols = 0;
	}

	// Depth
	if(!data.available.depthImageEnable)
	{
		data.depthImage.rows = 0;
		data.depthImage.cols = 0;
	}

	// Validity
	if(!data.available.validityImageEnable)
	{
		data.validityImage.rows = 0;
		data.validityImage.cols = 0;
	}

	// Table
	if(!data.available.testImageEnable)
	{
		data.testImage.rows = 0;
		data.testImage.cols = 0;
	}
	

	frameDataLength = byteNum;
	return;
}
*/

}
