#ifndef _SRCGCSub_h
#define _SRCGCSub_h 1
// header file generated from SRCGCSub.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCStar.h"

class SRCGCSub : public SRCGCStar
{
public:
	SRCGCSub();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock code;
	/* virtual */ int myExecTime();
	InSRCGCPort input1;
	InSRCGCPort input2;
	OutSRCGCPort output;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
};
#endif
