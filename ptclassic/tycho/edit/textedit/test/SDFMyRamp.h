#ifndef _SDFMyRamp_h
#define _SDFMyRamp_h 1
// header file generated from SDFMyRamp.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "SDFStar.h"
#include "FloatState.h"

class SDFMyRamp : public SDFStar
{
public:
	SDFMyRamp();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	OutSDFPort output;

protected:
	/* virtual */ void go();
	FloatState step;
	FloatState value;

};
#endif
