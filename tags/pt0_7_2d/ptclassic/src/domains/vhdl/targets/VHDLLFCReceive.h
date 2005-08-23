#ifndef _VHDLLFCReceive_h
#define _VHDLLFCReceive_h 1
// header file generated from VHDLLFCReceive.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1993-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "VHDLCSynchComm.h"

class VHDLLFCReceive : public VHDLCSynchComm
{
public:
	VHDLLFCReceive();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock C2VintegerConfig;
	static CodeBlock C2VrealConfig;
	/* virtual */ void begin();
	OutVHDLPort output;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
};
#endif
