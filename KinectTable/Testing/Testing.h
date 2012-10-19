#ifndef TESTING_H
#define TESTING_H

//
// This is a header to access the Testing module.
//

#ifdef MOD_TESTING
#error "Files inside module should not access this file."
#endif

#define MOD_TESTING

// Includes //
#include "Tests.h"

#undef MOD_TESTING

#endif