#ifndef NETWORK__INFO_H
#define NETWORK__INFO_H

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
#include "Serialization/Serialization.h"


// Standard C


// Namespaces //


namespace Network
{
	

	struct Info : public Serialization::ISerializable
	{
		static const size_t maxNameLength = 10;

		char name[maxNameLength];

		void Serialize(Serialization::Serializer& serializer) const
		{
			serializer.WriteList(name, maxNameLength);
			return;
		}

		void Deserialize(Serialization::Deserializer& deserializer)
		{
			unsigned long length = maxNameLength;
			deserializer.ReadList(name, length);
			return;
		}

	};

}


#endif
