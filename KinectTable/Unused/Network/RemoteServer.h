#ifndef NETWORK__REMOTE_SERVER_H
#define NETWORK__REMOTE_SERVER_H

//
// This is a virtualization of the server on the other end of a remote connection.
//


// Includes //


// Project
#include "DataTypes.h"
#include "NetworkFramer.h"


// Standard C++


// Standard C




// Namespaces //




namespace Network
{
	
	class RemoteClient
	{
		
	  public:

		RemoteClient();
		virtual ~RemoteClient();

		void Unsubscribe();
		bool IsSubscribed() const;

		
		void SetKinectData(const KinectData& data, const KinectDataParams& dataParams);

		void GetInfo(NetworkFramer::Info& info);


	  private:

		volatile NetworkFramer::Info info;
		volatile bool isSubscribed;

	};
	
}


#endif