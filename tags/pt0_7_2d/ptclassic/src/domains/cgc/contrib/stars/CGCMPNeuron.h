#ifndef _CGCMPNeuron_h
#define _CGCMPNeuron_h 1
// header file generated from CGCMPNeuron.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "CGCNeuronBase.h"
#include "FloatState.h"

class CGCMPNeuron : public CGCNeuronBase
{
public:
	CGCMPNeuron();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock final;
	/* virtual */ int myExecTime();
protected:
	/* virtual */ void go();
	FloatState threshold;

};
#endif
