#ifndef _CGCStar_h
#define _CGCStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee and J. T. Buck

 This is the baseclass for stars that generate C language code

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "CGCPortHole.h"

class CGCStar : public CGStar {
friend class CGCTarget;
public:
	CGCStar(): emptyFlag(1) {}

	// List of all states pointed to in the code.
	// This is public so that CGCTarget and other targets can access it.
	StateList referencedStates;

	// initialize method clears out referencedStates list
	void initialize();

	// my domain
	const char* domain() const;

	int run();

	// class identification
	int isA(const char*) const;

protected:
	// access to target (cast is safe: always a CGCTarget)
	CGCTarget* target() {
		return (CGCTarget*)myTarget();
	}

	// Virtual functions. Expand State or PortHole reference macros.
	// If "name" is a state, add it to the list of referenced states.
	StringList expandRef(const char* name);
	StringList expandRef(const char* name, const char* offset);

	// If "name" is a state, and is not already on the list
	// referencedStates, add it.
	void registerState(const char* name);

	// Add lines to be put at the beginning of the code file
	void addInclude(const char* decl);

	// Add declarations, to be put at the beginning of the main section
	void addDeclaration(const char* decl);

	// Add global declarations, to be put ahead of the main section
	void addGlobal(const char* decl);

	// Add procedures, to be put ahead of the main section
	void addProcedure(const char* decl);

	// Add main initializations, to be put at the beginning of the main 
	// section
	void addMainInit(const char* decl);

	// After each firing, update the offset pointers
	virtual void updateOffsets();

private:
	// define and initialize variables for C program.
	// Note that CGCTarget is a friend class to access these methods
	// freely.

	int emptyFlag;

	// declare PortHoles and States
	virtual StringList declarePortHole(CGCPortHole* p);
	virtual StringList declareOffset(const CGCPortHole* p);
	virtual StringList declareState(const State* p);

	// initialize PortHoles and States
	virtual StringList initializeBuffer(const CGCPortHole* p);
	virtual StringList initializeOffset(const CGCPortHole* p);
	virtual StringList initializeState(const State* p);

	// offset initialize
	void initBufPointer();

};

#endif
