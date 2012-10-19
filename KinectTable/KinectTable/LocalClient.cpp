//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "LocalClient.h"

// Module
#include "DataForwarder.h"

// Project
#include "Sensors/Sensors.h"
#include "Network/Network.h"

// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace KinectTable
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

LocalClient::LocalClient(const SessionParameters& sessionParameters) :
	sessionType(SESSION_TYPE_PRIVATE),
	dataProcessor(*new Sensors::DataProcessor(sessionParameters.dataParams)),
	localParams(sessionParameters.dataParams),
	server(NULL),
	frameNumGiven(0),
	frameNumTaken(0),
	dataMutex(),
	newDataEvent(CreateEvent(NULL, true, false, "")),
	dataForwarder(new DataForwarder(dataProcessor, this, NULL))
{
	return;
}

LocalClient::LocalClient(const SessionParameters& sessionParameters, const char* localAddress) :
	sessionType(SESSION_TYPE_PUBLIC),
	dataProcessor(*new Sensors::DataProcessor(sessionParameters.dataParams)),
	localParams(sessionParameters.dataParams),
	frameNumGiven(0),
	frameNumTaken(0),
	dataMutex(),
	newDataEvent(CreateEvent(NULL, true, false, ""))
{
	// Create server info
	Network::Info info;
	assert(sizeof("Hello") < Network::Info::maxNameLength);  //!!magic string
	memcpy(info.name, "Hello", sizeof(info.name)); //!!magic string

	// Get address
	Sockets::TcpSocket::IpAddress ipAddress;
	memcpy(ipAddress, localAddress, sizeof(ipAddress));

	// Create server
	server = new Network::Server(info, ipAddress, portNum);
	dataForwarder = new DataForwarder(dataProcessor, this, server);
	return;
}

LocalClient::~LocalClient()
{
	// Destroy data processor
	delete &dataProcessor;
	
	delete dataForwarder;

	// Destroy server
	if(sessionType == SESSION_TYPE_PUBLIC)
	{
		assert(server != NULL);
		delete server;
		server = NULL;
	}
	assert(server == NULL);

	CloseHandle(newDataEvent);
	return;
}

void LocalClient::GetParams(KinectDataParams& params)
{
	dataProcessor.GetParameters(params);
	return;
}


void LocalClient::SetParams(const KinectDataParams& params)
{
	// Set the local params
	localParams = params;

	// Tell the data processor what data to make
	switch(sessionType)
	{
		case SESSION_TYPE_PRIVATE:
			dataProcessor.SetParameters(localParams);
			break;

		case SESSION_TYPE_PUBLIC:
		{
			//!! What abput when a remote user changes its params?

			// Get data params of remote users
			KinectDataParams dataParams;
			server->GetDataParams(dataParams);

			// Find union between local user and remote users
			dataParams.colorImageEnable |= params.colorImageEnable;
			dataParams.depthImageEnable |= params.depthImageEnable;
			dataParams.validityImageEnable |= params.validityImageEnable;
			dataParams.testImageEnable |= params.testImageEnable;

			dataParams.tableEnable |= params.tableEnable;
			dataParams.handsEnable |= params.handsEnable;
			
			dataProcessor.SetParameters(dataParams);
			break;
		}

		default: assert(false); break;
	}

	return;
}

void LocalClient::RecalculateTableCorners()
{
	dataProcessor.RecalculateTableCorners();
	return;
}

bool LocalClient::GetData(KinectData& data, bool waitForNewData)
{

	if(waitForNewData)
		WaitForSingleObject(newDataEvent, INFINITE);

	bool frameChanged;
	dataMutex.Lock();
	{
		this->data.CopyTo(data);

		// Find intersection between the data the user wants and the data available
		KinectDataParams& params = data.available;
		params.colorImageEnable = localParams.colorImageEnable && params.colorImageEnable;
		params.depthImageEnable = localParams.depthImageEnable && params.depthImageEnable;
		params.validityImageEnable = localParams.validityImageEnable && params.validityImageEnable;
		params.testImageEnable = localParams.testImageEnable && params.testImageEnable;



		frameChanged = this->frameNumTaken != this->frameNumGiven;
		this->frameNumTaken = this->frameNumGiven;

		ResetEvent(newDataEvent);
	}
	dataMutex.Unlock();
	
	//!! What if the user changes the params and gets new data?
	return frameChanged;
}


void LocalClient::SetData(long frameNum, KinectData& data)
{
	dataMutex.Lock();
	{
		this->frameNumGiven = frameNum;
		data.CopyTo(this->data);
		SetEvent(newDataEvent);
	}
	dataMutex.Unlock();
}

//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------





}
