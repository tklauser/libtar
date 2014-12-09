/*
**  Copyright 2002-2003 University of Illinois Board of Trustees
**  Copyright 2002-2003 Mark D. Roth
**  All rights reserved.
**
**  internal.h - internal header file for libtar
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#ifndef INTERNAL_H
#define INTERNAL_H

#include <config.h>
#include <compat.h>

#include <libtar.h>

#ifdef TLS
#define TLS_THREAD TLS
#else
#define TLS_THREAD
#endif

char* safer_name_suffix(char const*);

#endif
