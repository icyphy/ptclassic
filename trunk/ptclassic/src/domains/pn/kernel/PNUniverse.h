/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 February 1992
*/

#ifndef _MTDFUniverse_h
#define _MTDFUniverse_h

#include "Universe.h"
#include "MTDFScheduler.h"

const char MTDFstring[] = "MULTI-THREADED DATAFLOW";
	
class MTDFUniverse : public Universe
{
public:
    MTDFUniverse() : Universe(new MTDFScheduler,MTDFstring) {}
};

#endif
