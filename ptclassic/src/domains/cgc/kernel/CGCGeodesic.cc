static const char file_id[] = "CGCGeodesic.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 geodesics for CGCCode stars.  In addition to the functions of CG
 geodesics, they manage memory allocation for buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCGeodesic.h"
#include "CGCConnect.h"
#include "Error.h"

ISA_FUNC(CGCGeodesic,CGGeodesic);

// Return the name assigned to the buffer.
// If I am a fork destination, my buffer name is that of the source.
char* CGCGeodesic::getBufName() const {
	const CGCPortHole* p = (const CGCPortHole*)src();
	return p ? p->geo().getBufName() : bufName;
}
