#ifndef _CGCSigmoid_h
#define _CGCSigmoid_h 1
// header file generated from CGCSigmoid.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1997 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "CGCStar.h"
#include "FloatState.h"

class CGCSigmoid : public CGCStar
{
public:
	CGCSigmoid();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock exp;
	/* virtual */ int myExecTime();
	/* virtual */ void initCode();
	InCGCPort input;
	OutCGCPort output;

protected:
	/* virtual */ void go();
	FloatState learning_rate;

};
#endif
