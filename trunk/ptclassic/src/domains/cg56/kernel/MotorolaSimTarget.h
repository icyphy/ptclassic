#ifndef _MotorolaSimTarget_h
#define _MotorolaSimTarget_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino and J. Buck

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmTarget.h"

class MotorolaSimTarget {
private:
	StringList& assembleCmds
	AsmTarget* simTargetPtr;
public:
	void initStates();
	void initializeCmds(const char* dspType); // either 56 or 96
	int wrapup(const char* startAddress, const char* finalJumpLocation);

#endif
