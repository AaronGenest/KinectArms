#ifndef DATA_TYPES_H
#define DATA_TYPES_H

//
// This is a header to access the DataTypes module.
//

//!!#ifdef MOD_DATA_TYPES
//#error "Files inside module should not access this file."
//#endif


#define MOD_DATA_TYPES

// Includes //
#include "carray.h"
#include "carray2d.h"
#include "Constants.h"
#include "Hand.h"
#include "Images.h"
#include "KinectData.h"
#include "KinectDataParams.h"
#include "Pixels.h"
#include "Points.h"
#include "Table.h"
#include "Other.h"

#undef MOD_DATA_TYPES

//!!
using namespace DataTypes;

#endif
