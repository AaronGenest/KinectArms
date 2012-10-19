#ifndef NETWORK__FRAMES_H
#define NETWORK__FRAMES_H

//
// This holds all the network frames we can send over the network.
//


// Includes //


// Project
#include "Framer.h"

// Standard C++


// Standard C
#include <assert.h>



// Namespaces //




namespace Network
{


// Data //



//---------------------------------------------------------------------------
// Struct Interfaces
//---------------------------------------------------------------------------


//Interface of a serializable (tightly-packed) version of the DataParams structure
struct DataParams
{

	// Accessors
	
	const bool& ColorImageEnable() const { return (bool&)byteArray[0]; }
	const bool& DepthImageEnable() const { return (bool&)byteArray[1]; }
	const bool& ValidityImageEnable() const { return (bool&)byteArray[2]; }
	const bool& TableImageEnable() const { return (bool&)byteArray[3]; }

	
	// Mutators

	bool& ColorImageEnable() { return (bool&)byteArray[0]; }
	bool& DepthImageEnable() { return (bool&)byteArray[1]; }
	bool& ValidityImageEnable() { return (bool&)byteArray[2]; }
	bool& TableImageEnable() { return (bool&)byteArray[3]; }

  private:
	static const size_t size = 4*sizeof(bool);
	char byteArray[size];
};



template<typename T>
struct ImageData
{
	
	ImageData()
	{
		assert(sizeof(Rows()) == 2);
		assert(sizeof(Cols()) == 2);
		assert(sizeof(DataLength()) == 4);
		return;
	}
	
	// Accessors
	const unsigned short& Rows() const { return (unsigned short&)byteArray[0]; }
	const unsigned short& Cols() const { return (unsigned short&)byteArray[2]; }
	const bool& Compressed() const { return (bool&)byteArray[4]; }

	const unsigned long& DataLength() const { return (unsigned long&)byteArray[5]; }
	const char* const Data() const { return (const char*)&byteArray[9]; }
	
	const size_t GetSize() const { return sizeof(unsigned short) + sizeof(unsigned short) + sizeof(bool) + sizeof(unsigned long) + DataLength(); }

	// Mutators

	unsigned short& Rows() { return (unsigned short&)byteArray[0]; }
	unsigned short& Cols() { return (unsigned short&)byteArray[2]; }
	bool& Compressed() { return (bool&)byteArray[4]; }

	unsigned long& DataLength() { return (unsigned long&)byteArray[5]; }
	char* const Data() { return (char*)&byteArray[9]; }


  private:

	static const size_t maxSize = sizeof(unsigned short) + sizeof(unsigned short) + sizeof(bool) + sizeof(unsigned long) + Types<T>::FlatImageBytes::maxSize;
	char byteArray[maxSize];

};



//---------------------------------------------------------------------------
// Frames
//---------------------------------------------------------------------------


struct Frame
{

  private:
	
    typedef char ByteArray[1];

  protected:

	Frame()
	{
		assert(sizeof(FrameType()) == 1);
		return;
	}

	FrameType::Type& FrameType() { return (FrameType::Type&)((ByteArray&)*this)[0]; }

	static const size_t size = sizeof(FrameType::Type);

  public:

	const FrameType::Type& FrameType() const { return (FrameType::Type&)((ByteArray&)*this)[0]; }

	
};


struct SubscribeFrame : Frame
{
	
	SubscribeFrame()
	{
		FrameType() = (FrameType::Type)FrameType::Subscribe;
		return;
	}
	
	const Network::Info& Info() const { return (Network::Info&)byteArray[infoStart]; }
	const DataParams& DataParams() const { return (Network::DataParams&)byteArray[infoStart + sizeof(Network::Info)]; }

	Network::Info& Info() { return (Network::Info&)byteArray[infoStart]; }
	Network::DataParams& DataParams() { return (Network::DataParams&)byteArray[infoStart + sizeof(Network::Info)]; }
	

  private:

	  static const size_t infoStart = Frame::size;

	  static const size_t size = Frame::size + sizeof(Network::Info) + sizeof(Network::DataParams);
	  char byteArray[size];
};



struct DataParamsFrame : Frame
{
	
	DataParamsFrame()
	{
		FrameType() = (FrameType::Type)FrameType::DataParams;
		return;
	}
	

	const Network::DataParams& DataParams() const { return (Network::DataParams&)byteArray[infoStart]; }
	Network::DataParams& DataParams() { return (Network::DataParams&)byteArray[infoStart]; }
	

  private:

	  static const size_t infoStart = Frame::size;

	  static const size_t size = Frame::size + sizeof(Network::DataParams);
	  char byteArray[size];
};





struct SubscribeAckFrame : Frame
{
	
	SubscribeAckFrame()
	{
		FrameType() = (FrameType::Type)FrameType::SubscribeAck;
		return;
	}
	

	const Network::Info& Info() const { return (Network::Info&)byteArray[infoStart]; }
	const bool& Accept() const { return (bool&)byteArray[infoStart + sizeof(Network::Info)]; }

	Network::Info& Info() { return (Network::Info&)byteArray[infoStart]; }
	bool& Accept() { return (bool&)byteArray[infoStart + sizeof(Network::Info)]; }
	

  private:

	  static const size_t infoStart = Frame::size;

	  static const size_t size = Frame::size + sizeof(Network::Info) + sizeof(bool);
	  char byteArray[size];
};








//NOTE: Images must be set in order!
struct KinectDataFrame : Frame
{
	
	KinectDataFrame()
	{
		FrameType() = (FrameType::Type)FrameType::Data;
		return;
	}

	
	// Accessors //

	const ImageData<ColorPixel>& ColorImage() const { return (ImageData<ColorPixel>&)byteArray[ColorImageStart()]; }
	const ImageData<DepthPixel>& DepthImage() const { return (ImageData<DepthPixel>&)byteArray[DepthImageStart()]; }
	const ImageData<BinaryPixel>& ValidityImage() const { return (ImageData<BinaryPixel>&)byteArray[ValidityImageStart()]; }
	const ImageData<BinaryPixel>& TableImage() const { return (ImageData<BinaryPixel>&)byteArray[TableImageStart()]; }


	// Mutators //

	ImageData<ColorPixel>& ColorImage() { return (ImageData<ColorPixel>&)byteArray[ColorImageStart()]; }
	ImageData<DepthPixel>& DepthImage() { return (ImageData<DepthPixel>&)byteArray[DepthImageStart()]; }
	ImageData<BinaryPixel>& ValidityImage() { return (ImageData<BinaryPixel>&)byteArray[ValidityImageStart()]; }
	ImageData<BinaryPixel>& TableImage() { return (ImageData<BinaryPixel>&)byteArray[TableImageStart()]; }


  private:

	  static const size_t infoStart = Frame::size;

	  size_t ColorImageStart() const { return infoStart; }
	  size_t DepthImageStart() const { return ColorImageStart() + ColorImage().GetSize(); }
	  size_t ValidityImageStart() const { return DepthImageStart() + DepthImage().GetSize(); }
	  size_t TableImageStart() const { return ValidityImageStart() + ValidityImage().GetSize(); }
	  
	  static const size_t maxSize = Frame::size + sizeof(ImageData<ColorPixel>) + sizeof(ImageData<DepthPixel>) + 2 * sizeof(ImageData<BinaryPixel>);
	  char byteArray[maxSize];
};




struct UnsubscribeFrame : Frame
{
	
	UnsubscribeFrame()
	{
		FrameType() = (FrameType::Type)FrameType::Unsubscribe;
		return;
	}
	

  private:

	  static const size_t size = Frame::size;
	  char byteArray[size];
};


struct InfoRequestFrame : Frame
{
	
	InfoRequestFrame()
	{
		FrameType() = (FrameType::Type)FrameType::InfoRequest;
		return;
	}
	

  private:

	  static const size_t size = Frame::size;
	  char byteArray[size];
};

	

struct InfoFrame : Frame
{
	
	InfoFrame()
	{
		FrameType() = (FrameType::Type)FrameType::Info;
		return;
	}
	

	const Network::Info& Info() const { return (Network::Info&)byteArray[infoStart]; }
	Network::Info& Info() { return (Network::Info&)byteArray[infoStart]; }
	

  private:

	  static const size_t infoStart = Frame::size;

	  static const size_t size = Frame::size + sizeof(Network::Info);
	  char byteArray[size];
};


	
}


#endif
