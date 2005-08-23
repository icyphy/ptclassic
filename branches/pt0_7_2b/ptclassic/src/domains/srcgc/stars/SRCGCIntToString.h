#ifndef _SRCGCIntToString_h
#define _SRCGCIntToString_h 1
// header file generated from SRCGCIntToString.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCStar.h"

class SRCGCIntToString : public SRCGCStar
{
public:
	SRCGCIntToString();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock code;
	InSRCGCPort input;
	OutSRCGCPort output;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
};
#endif
