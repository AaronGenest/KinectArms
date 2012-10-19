#ifndef SENSORS__DATA_PROCESSOR_H
#define SENSORS__DATA_PROCESSOR_H

// Module Check //
#ifndef MOD_SENSORS
#error "Files outside module cannot access file directly."
#endif


//
// This processes data from the Kinect.
//


// Includes //


// Project
#include "KinectSensor.h"
#include "DataTypes/DataTypes.h"
#include "Threading/Threading.h"
#include "ImageProcessing/ImageProcessing.h"

// Standard C++


// Standard C




// Namespaces //



namespace Sensors
{


	//!! Should process an image in the constructor.
	//!! Should be a singleton right now until we support multiple Kinects!
	class DataProcessor
	{

	  public:


		DataProcessor(const KinectDataParams& params);
		virtual ~DataProcessor();


		// Gets which data the sensor should calculate from each frame
		void GetParameters(KinectDataParams& params);

		// Sets which data the sensor should calculate from each frame
		void SetParameters(const KinectDataParams& params);


		// Recalculates the table corners from the next image
		void RecalculateTableCorners();


		// Gets the current data available
		// Returns the frame number.
		long GetData(KinectData& data, bool waitForNewData = false);

		// Returns whether the sensor is connected.
		//!!bool IsConnected() const;

	  private:

		int ProcessingLoop();

		//!!
		void CreateTestImage(KinectData& data);


		// Data the user wants
		KinectDataParams userParams;
		bool calculateTableCorners;

		// Data available
		KinectData data;
		Table table;
		long frameNumber;

		const HANDLE newDataEvent;

		ImageProcessing::HandLabeler handLabeler;

		// Thread stuff
		static int ThreadStart(void* obj);
		Threading::Thread thread;
		volatile bool threadExit;

		// A mutex to be used for setting parameters and getting data
		Threading::Mutex mutex;

	};

}


#endif