#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

//
// This is a header to access the ImageProcessing module.
//

#ifdef MOD_IMAGE_PROCESSING
#error "Files inside module should not access this file."
#endif

#define MOD_IMAGE_PROCESSING

// Includes //
#include "HandDetection.h"
#include "HandLabeler.h"
#include "Helpers.h"
#include "TableDetection.h"

#undef MOD_IMAGE_PROCESSING

#endif