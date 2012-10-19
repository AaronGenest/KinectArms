#ifndef CODECS_H
#define CODECS_H

//
// This is a header to access the Template module.
//

#ifdef MOD_CODECS
#error "Files inside module should not access this file."
#endif


#define MOD_CODECS

// Includes //
#include "RunLengthEncoder.h"
#include "ZlibEncoder.h"

#undef MOD_CODECS

#endif