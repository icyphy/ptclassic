static const char file_id[] = "VHDLBGeodesic.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Edward A. Lee

 geodesics for VHDLB stars.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLBGeodesic.h"
#include "VHDLBPortHole.h"
#include "Error.h"

ISA_FUNC(VHDLBGeodesic,CGGeodesic);

// Return the name assigned to the buffer.
// If I am a fork destination, my buffer name is that of the source.
// Need to change this for the VHDLB code to be meaningful
char* VHDLBGeodesic::getBufName() const {
	const VHDLBPortHole* p = (const VHDLBPortHole*)src();
	return p ? p->geo().getBufName() : bufName;
}
