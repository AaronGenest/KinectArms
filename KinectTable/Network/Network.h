#ifndef NETWORK_H
#define NETWORK_H

//
// This is a header to access the Network namespace.
//

#ifdef MOD_NETWORK
#error "Files inside module should not access this file."
#endif


#define MOD_NETWORK

// Includes //
#include "Client.h"
#include "Server.h"

#undef MOD_NETWORK

#endif