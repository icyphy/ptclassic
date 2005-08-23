#ifndef _CGCNeuron_h
#define _CGCNeuron_h 1
// header file generated from CGCNeuron.pl by ptlang

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

class CGCNeuron : public CGCNeuronBase
{
public:
	CGCNeuron();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock final;
protected:
	/* virtual */ void go();
};
#endif
