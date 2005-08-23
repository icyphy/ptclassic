#ifndef _CGCNeuronBase_h
#define _CGCNeuronBase_h 1
// header file generated from CGCNeuronBase.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "CGCStar.h"
#include "FloatArrayState.h"

class CGCNeuronBase : public CGCStar
{
public:
	CGCNeuronBase();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock initial;
	const char* iteration(int k);
	/* virtual */ int myExecTime();
	MultiInCGCPort input;
	OutCGCPort output;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
	FloatArrayState weights;

};
#endif
