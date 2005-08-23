#ifndef _SRCGCWhen_h
#define _SRCGCWhen_h 1
// header file generated from SRCGCWhen.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCNonStrictStar.h"

class SRCGCWhen : public SRCGCNonStrictStar
{
public:
	SRCGCWhen();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock code;
	/* virtual */ int myExecTime();
	InSRCGCPort input;
	InSRCGCPort clock;
	OutSRCGCPort output;

protected:
	/* virtual */ void go();
};
#endif
