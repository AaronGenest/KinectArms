#ifndef SENSORS_H
#define SENSORS_H

//
// This is a header to access the Sensor module.
//

#ifdef MOD_SENSORS
#error "Files inside module should not access this file."
#endif


#define MOD_SENSORS

// Includes //
#include "DataProcessor.h"

#undef MOD_SENSORS

#endif