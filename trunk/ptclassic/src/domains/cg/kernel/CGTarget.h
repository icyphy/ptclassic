/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

 Baseclass for all single-processor code generation targets.

*******************************************************************/

#ifndef _CGTarget_h
#define  _CGTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "StringState.h"

class CGStar;

class UserOutput;

class CGTarget : public Target {
protected:
	StringList myCode;

	// The following utilities are used by many types of code generators
	// Return a list of spaces for indenting
	StringList indent(int depth);

	// calculate the buffer request.
	virtual int decideBufSize(Galaxy&);

	// do initialization: say compute offsets of portholes and
	// generate initCode. But in this base class, do nothing
	virtual int codeGenInit(Galaxy&);

public:
	CGTarget(const char* name, const char* starclass, const char* desc,
		 char sep = '_');
	void initialize();
	// The setup method should not be invoked if the stars are not
	// CGStars or their portHoles are not CGPortHoles.
	int setup(Galaxy&);
	void start();
	int run();
	void wrapup();
	Block* clone() const;
	void addCode(const char*);
	virtual void headerCode();
	virtual void writeCode(UserOutput&);
	int isA(const char*) const;

	// Total Number of Labels generated.
	int numLabels;
	// Label Separator
	char separator;

	// output a comment.  Default form uses C-style comments.
	virtual void outputComment (const char*);

	// destructor
	~CGTarget();
};

// Class for unique symbol generation.  The symbol is guaranteed
// to be unique with respect to all other symbols in the target.
class Symbol {
	private:
		// List of all symbols
		StringList symbols;
		CGTarget* myTarget;
	public:
		Symbol(CGTarget* t = 0);
		void initialize() { symbols.initialize(); };
		StringList lookup(const char* name);
		void setTarget(CGTarget* t) {myTarget = t;}
};

#endif
