#ifndef TEMPLATE_H
#define TEMPLATE_H

//
// This is a header to access the Template module.
//

#ifdef MOD_TEMPLATE
#error "Files inside module should not access this file."
#endif


#define MOD_TEMPLATE

// Includes //
#include "TemplateClass.h"

#undef MOD_TEMPLATE

#endif