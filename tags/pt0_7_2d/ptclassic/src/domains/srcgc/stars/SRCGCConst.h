#ifndef _SRCGCConst_h
#define _SRCGCConst_h 1
// header file generated from SRCGCConst.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCStar.h"
#include "IntState.h"

class SRCGCConst : public SRCGCStar
{
public:
	SRCGCConst();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ int myExecTime();
	OutSRCGCPort output;

protected:
	/* virtual */ void go();
	IntState level;

};
#endif
