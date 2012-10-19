#ifndef RANDOMIZATION__I_RANDOMIZABLE_H
#define RANDOMIZATION__I_RANDOMIZABLE_H


// Module Check //
#ifndef MOD_RANDOMIZATION
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




namespace Randomization
{

	class Randomizer;

	// Randomizes all the members of the object.
	class IRandomizable
	{

	  public:

		virtual ~IRandomizable() { }

		
		// Randomizes the object
		virtual void Randomize(Randomizer& randomizer) = 0;
		  
	};

}


#endif
