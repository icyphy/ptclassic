#ifndef _VHDLLFCSend_h
#define _VHDLLFCSend_h 1
// header file generated from VHDLLFCSend.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1993-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright	for copyright notice,
limitation of liability, and disclaimer	of warranty provisions.
 */
#include "VHDLCSynchComm.h"

class VHDLLFCSend : public VHDLCSynchComm
{
public:
	VHDLLFCSend();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock V2CintegerConfig;
	static CodeBlock V2CrealConfig;
	/* virtual */ void begin();
	InVHDLPort input;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
};
#endif
