#ifndef SERIALIZATION__I_SERIALIZABLE_H
#define SERIALIZATION__I_SERIALIZABLE_H


// Module Check //
#ifndef MOD_SERIALIZATION
#error "Files outside module cannot access file directly."
#endif



//
// This is an interface for serializable objects.
//


// Includes //


// Project


// Standard C++


// Standard C




// Namespaces //




namespace Serialization
{

	class Serializer;
	class Deserializer;


	// Converts the object into a stream of bytes.
	class ISerializable
	{

	  public:

		virtual ~ISerializable() { }

		
		// Max size specifies how large the data array is and is set to how much of the data array was used.
		virtual void Serialize(Serialization::Serializer& serializer) const = 0;

		// Max size specifies how large the data array is and is set to how much of the data array was used.
		virtual void Deserialize(Deserializer& deserializer) = 0;
		  
	};

}


#endif
