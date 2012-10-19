//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "RemoteClient.h"

// Project
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

RemoteClient::RemoteClient(const SessionParameters& sessionParameters, const char* remoteAddress) :
	localParams(sessionParameters.dataParams),
	frameNum(-1)
{

	// Set up client info
	Network::Info info;
	assert(sizeof("Client") < Network::Info::maxNameLength);  //!!magic string
	memcpy(info.name, "Client", sizeof(info.name)); //!!magic string


	// Connect a client to the server
	client = new Network::Client(remoteAddress, portNum);
	int ret = client->Subscribe(info, localParams);
	if(ret != 0)
		throw ret;

	return;
}


RemoteClient::~RemoteClient()
{
	
	// Destroy client
	assert(client != NULL);
	delete client;
	client = NULL;
	
	return;
}

void RemoteClient::GetParams(KinectDataParams& params)
{
	params = localParams;
	return;
}

void RemoteClient::SetParams(const KinectDataParams& params)
{
	// Set the local params
	localParams = params;

	client->SetKinectDataParams(params);
	return;
}

void RemoteClient::RecalculateTableCorners()
{
	throw ENOTSUP;
}

bool RemoteClient::GetData(KinectData& data, bool waitForNewData)
{
	// Get data
	long frameNum = client->GetKinectData(data, waitForNewData);

	// Find intersection between the data the user wants and the data available
	KinectDataParams& params = data.available;
	params.colorImageEnable = localParams.colorImageEnable && params.colorImageEnable;
	params.depthImageEnable = localParams.depthImageEnable && params.depthImageEnable;
	params.validityImageEnable = localParams.validityImageEnable && params.validityImageEnable;
	params.testImageEnable = localParams.testImageEnable && params.testImageEnable;

	return frameNum != this->frameNum;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------





}
