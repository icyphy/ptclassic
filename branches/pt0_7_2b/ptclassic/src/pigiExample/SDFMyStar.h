#ifndef _SDFMyStar_h
#define _SDFMyStar_h 1
// header file generated from SDFMyStar.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "SDFStar.h"

class SDFMyStar : public SDFStar
{
public:
	SDFMyStar();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	InSDFPort input;
	OutSDFPort output;

protected:
	/* virtual */ void go();
};
#endif
