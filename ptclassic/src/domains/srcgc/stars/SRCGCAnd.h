#ifndef _SRCGCAnd_h
#define _SRCGCAnd_h 1
// header file generated from SRCGCAnd.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCNonStrictStar.h"

class SRCGCAnd : public SRCGCNonStrictStar
{
public:
	SRCGCAnd();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock code;
	/* virtual */ int myExecTime();
	InSRCGCPort input1;
	InSRCGCPort input2;
	OutSRCGCPort output;

protected:
	/* virtual */ void go();
};
#endif
