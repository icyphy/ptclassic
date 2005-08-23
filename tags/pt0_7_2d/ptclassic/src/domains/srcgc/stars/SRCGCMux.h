#ifndef _SRCGCMux_h
#define _SRCGCMux_h 1
// header file generated from SRCGCMux.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCNonStrictStar.h"

class SRCGCMux : public SRCGCNonStrictStar
{
public:
	SRCGCMux();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock code;
	InSRCGCPort trueInput;
	InSRCGCPort falseInput;
	InSRCGCPort select;
	OutSRCGCPort output;

protected:
	/* virtual */ void go();
};
#endif
