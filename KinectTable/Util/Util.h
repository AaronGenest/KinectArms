#ifndef UTIL_H
#define UTIL_H

//
// This is a header to access the Util module.
//

//!!This shouldn't be used because Util is not a self-contained module
#error "This shouldn't be used because Util is not a self-contained module."

#ifdef MOD_UTIL
#error "Files inside module should not access this file."
#endif


#define MOD_UTIL

// Includes //
#include "DataExporter.h"
#include "Drawing.h"
#include "ISerializable.h"
#include "ISizable.h"
#include "Helpers.h"

#undef MOD_UTIL

#endif