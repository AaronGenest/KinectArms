#ifndef SOCKETS_H
#define SOCKETS_H

//
// This is a header to access the Sockets namespace.
//

#ifdef MOD_SOCKETS
#error "Files inside module should not access this file."
#endif


#define MOD_SOCKETS

// Includes //
#include "TcpSocket.h"
#include "TcpListener.h"

#undef MOD_SOCKETS

#endif