#ifndef UTIL__SHARED_VAR_H
#define UTIL__SHARED_VAR_H

//
// This is a data type that guarantees to be mutually exclusive.
//


// Includes //


// Project
#include "Mutex.h"

// Standard C++


// Standard C




// Namespaces //




namespace Util
{
	
	//!! Just here for playing around.  Shouldn't be used.
	template<typename T>
	class SharedVar
	{
		
	  public:

		SharedVar<T>& operator=(const T& var)
		{
			mutex.Lock();
			{
				value = var;
			}
			mutex.Unlock();

			return *this;
		}

		SharedVar<T>& operator=(const SharedVar<T>& var)
		{
			if(this == &var)
				return *this;
			
			mutex.Lock();
			{
				value = var;
			}
			mutex.Unlock();

			return *this;
		}

		// Need all the other operator overloads...


		void GetValue(T& var)
		{
			mutex.Lock();
			{
				var = value;
			}
			mutex.Unlock();

			return;
		}

	  private:
		
		T value;
		Util::Mutex mutex;
	};
	
}


#endif