/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck

 Symbol stuff.

*******************************************************************/
#ifndef _CGSymbol_h
#define _CGSymbol_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "StringStack.h"
#include "StringList.h"

class CGTarget;

// Class for unique symbol generation.  The symbol is guaranteed
// to be unique with respect to all other symbols in the target.
class Symbol {
private:
	// List of all symbols
	StringList symbols;
	CGTarget* myTarget;
public:
	Symbol(CGTarget* t=0);
	void initialize() { symbols.initialize(); }
	StringList lookup(const char*);
	void setTarget(CGTarget* t) {myTarget = t;}
};

// Class for unique nested symbol generation.
class NestedSymbol {
private:
	// List of all symbols
	StringStack symbols;
	CGTarget* myTarget;
public:
	NestedSymbol(CGTarget* t=0);
	void initialize() { symbols.initialize(); }
	const char* push(const char* tag="L");
	const char* pop();
	int depth() { return symbols.depth();}
	void setTarget(CGTarget* t) {myTarget = t;}
};
#endif
