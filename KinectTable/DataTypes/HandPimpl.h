#ifndef DATA_TYPES__HAND_PIMPL_H
#define DATA_TYPES__HAND_PIMPL_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif


//
// This keeps the OpenCV stuff in Hand to not be seen outside the library.
//


// Includes //


// Project
#include "Points.h"
#include "Images.h"
#include "Serialization/Serialization.h"

// OpenCV
#include <opencv2\opencv.hpp>

// Standard C++
#include <vector>

// Standard C
#include <assert.h>



// Namespaces //




namespace DataTypes
{
	
	struct HandPimpl : public Serialization::ISerializable
	{

	  public:
		HandPimpl() :
			boundaryStorage(cvCreateMemStorage(0)),
			cvBoundary(NULL)
		{
			return;
		}

		HandPimpl(const HandPimpl& handPimpl) : 
			boundaryStorage(cvCreateMemStorage(0))
		{
			*this = handPimpl;
			return;
		}

		HandPimpl(const CvSeq& cvBoundary) :
			boundaryStorage(cvCreateMemStorage(0))
		{
			SetBoundary(cvBoundary);
			return;
		}

		virtual ~HandPimpl()
		{
			cvReleaseMemStorage(const_cast<CvMemStorage**>(&boundaryStorage));
			return;
		}

		HandPimpl& operator=(const HandPimpl& handPimpl)
		{
			if(this == &handPimpl)
				return *this;

			if(handPimpl.cvBoundary != NULL)
				SetBoundary(*handPimpl.cvBoundary);
			else
				this->cvBoundary = NULL;

			return *this;
		}

		void SetBoundary(const CvSeq& cvBoundary)
		{
			this->cvBoundary = cvCloneSeq(&cvBoundary, boundaryStorage);
			return;
		}

		const CvSeq* GetBoundary() { return cvBoundary; }

		void Serialize(Serialization::Serializer& serializer) const
		{
			throw ENOTSUP;
		}
		
		void Deserialize(Serialization::Deserializer& deserializer)
		{
			throw ENOTSUP;
		}
		
	  private:

		CvMemStorage* const boundaryStorage;
		const CvSeq* cvBoundary;
	};


}


#endif