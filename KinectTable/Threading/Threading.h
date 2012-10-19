#ifndef THREADING_H
#define THREADING_H

//
// This is a header to access the Threading namespace.
//

#ifdef MOD_THREADING
#error "Files inside module should not access this file."
#endif


#define MOD_THREADING

// Includes //
#include "Thread.h"
#include "Mutex.h"

#undef MOD_THREADING

#endif