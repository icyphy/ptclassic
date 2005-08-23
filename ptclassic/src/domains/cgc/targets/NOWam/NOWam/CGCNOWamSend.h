#ifndef _CGCNOWamSend_h
#define _CGCNOWamSend_h 1
// header file generated from CGCNOWamSend.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1995-1997 The Regents of the University of California
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "CGCStar.h"
#include "CGCNOWamTarget.h"
#include "IntState.h"
#include "IntArrayState.h"

class CGCNOWamSend : public CGCStar
{
public:
	CGCNOWamSend();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock converttype;
	static CodeBlock timeincludes;
	static CodeBlock replyHandler;
	static CodeBlock errorHandler;
	static CodeBlock amdecls;
	static CodeBlock timedecls;
	static CodeBlock stardecls;
	static CodeBlock aminit;
	static CodeBlock timeinit;
	static CodeBlock starinit;
	static CodeBlock block;
	static CodeBlock runtime;
	/* virtual */ void wrapup();
	/* virtual */ void initCode();
	InCGCPort input;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
	IntState numData;
	IntArrayState nodeIPs;
	IntState hostAddr;
	IntState numNodes;
	IntState pairNumber;

private:
# line 20 "CGCNOWamSend.pl"
friend class CGCNOWamTarget;
		CGStar* partner;

};
#endif
