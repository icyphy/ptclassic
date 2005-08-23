#ifndef _SRCGCPre_h
#define _SRCGCPre_h 1
// header file generated from SRCGCPre.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCNonStrictStar.h"
#include "IntState.h"

class SRCGCPre : public SRCGCNonStrictStar
{
public:
	SRCGCPre();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	InSRCGCPort input;
	OutSRCGCPort output;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
	/* virtual */ void tick();
	IntState theState;

};
#endif
