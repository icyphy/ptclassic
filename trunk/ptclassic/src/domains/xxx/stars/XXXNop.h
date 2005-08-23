#ifndef _XXXNop_h
#define _XXXNop_h 1
// header file generated from XXXNop.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "XXXStar.h"

class XXXNop : public XXXStar
{
public:
	XXXNop();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	OutXXXPort output;
	InXXXPort input;

protected:
	/* virtual */ void go();
};
#endif
