//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_DATA_TYPES

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Hand.h"

// Project
#include "HandPimpl.h"

// OpenCV
#include <opencv2\opencv.hpp>

// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace DataTypes
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

Hand::Hand() :
	handPimpl(new HandPimpl())
{
	return;
}

Hand::Hand(const Hand& hand) :
	handPimpl(new HandPimpl())
{
	*this = hand;
	return;
}

Hand::~Hand()
{
	delete (HandPimpl*)handPimpl;
}

void Hand::CreateArmBlob(BinaryImage& armBlob, bool fillBlob) const
{
	// Clear image data
	armBlob.rows = armBlob.maxRows;
	armBlob.cols = armBlob.maxCols;
	memset(armBlob.data, 0, sizeof(armBlob.data));
	
	// Draw boundary of arm
	int thickness = fillBlob ? CV_FILLED : 1;
	IplImage blobImage = cv::Mat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)armBlob.data);
	CvScalar color = cv::Scalar(255);
	cvDrawContours(&blobImage, const_cast<CvSeq*>(((HandPimpl*)handPimpl)->GetBoundary()), color, color, 0, thickness);

	return;
}


Hand& Hand::operator=(const Hand& hand)
{
	if(this == &hand)
		return *this;
	
	id = hand.id;
	armBase = hand.armBase;
	boundary = hand.boundary;
	fingerTips = hand.fingerTips;
	fingerBases = hand.fingerBases;

	palmCenter = hand.palmCenter;

	// Arm properties
	meanDepth = hand.meanDepth;
	area = hand.area;
	centroid = hand.centroid;

	HandPimpl& thisHandPimpl = *((HandPimpl*)handPimpl);
	thisHandPimpl = *((HandPimpl*)hand.handPimpl);

	return *this;
}


bool Hand::operator==(const Hand& hand) const
{
	bool isEqual = true;

	isEqual &= (id == hand.id);

	isEqual &= (armBase == hand.armBase);
	isEqual &= (boundary == hand.boundary);
	isEqual &= (fingerTips == hand.fingerTips);
	isEqual &= (fingerBases == hand.fingerBases);

	isEqual &= (palmCenter == hand.palmCenter);

	isEqual &= (meanDepth == hand.meanDepth);
	isEqual &= (area == hand.area);
	isEqual &= (centroid == hand.centroid);

	return isEqual;
}

bool Hand::operator!=(const Hand& hand) const
{
	return !(*this == hand);
}

void Hand::Serialize(Serialization::Serializer& serializer) const
{

	serializer.Write(id);

	serializer.Write(armBase);
	serializer.WriteVector(boundary);
	serializer.WriteVector(fingerTips);
	serializer.WriteVector(fingerBases);

	serializer.Write(palmCenter);

	serializer.Write(meanDepth);
	serializer.Write(area);
	serializer.Write(centroid);
	

	// PImpl
	HandPimpl& thisHandPimpl = *((HandPimpl*)handPimpl);
	//!! serializer.Write(thisHandPimpl);
	
	return;
}

void Hand::Deserialize(Serialization::Deserializer& deserializer)
{

	deserializer.Read(id);

	deserializer.Read(armBase);
	deserializer.ReadVector(boundary);
	deserializer.ReadVector(fingerTips);
	deserializer.ReadVector(fingerBases);

	deserializer.Read(palmCenter);

	deserializer.Read(meanDepth);
	deserializer.Read(area);
	deserializer.Read(centroid);

	// PImpl
	HandPimpl& thisHandPimpl = *((HandPimpl*)handPimpl);
	//!! deserializer.Read(thisHandPimpl);

	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------
		


}
