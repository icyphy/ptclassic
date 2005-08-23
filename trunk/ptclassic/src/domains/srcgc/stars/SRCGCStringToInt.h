#ifndef _SRCGCStringToInt_h
#define _SRCGCStringToInt_h 1
// header file generated from SRCGCStringToInt.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Sup�lec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCStar.h"

class SRCGCStringToInt : public SRCGCStar
{
public:
	SRCGCStringToInt();
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
