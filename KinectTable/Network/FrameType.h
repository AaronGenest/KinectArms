#ifndef NETWORK__FRAME_TYPE_H
#define NETWORK__FRAME_TYPE_H

// Module Check //
#ifndef MOD_NETWORK
#error "Files outside module cannot access file directly."
#endif


//
// These functions send and receive user requests on the network.
//


// Includes //

// Project
#include "DataTypes/DataTypes.h"


// Standard C


// Namespaces //


namespace Network
{
	
	// Data //

	struct FrameType
	{
		typedef char Type;

		typedef enum
		{
			// Server
			Subscribe = 0,
			DataParams,
			//SpecialDataRequest,

			// Client
			SubscribeAck,
			Data,
			//SpecialData,

			// Common
			Unsubscribe,
			Info,
			InfoRequest

		};


		private: FrameType() {}
	};
	

}

#endif
