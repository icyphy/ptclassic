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
#include "CGCConnect.h"
#define CGCCodeBlock CodeBlock

// C - specific attributes.

const bitWord AB_XMEM = 0x10000; // allocate in X memory bank
const bitWord AB_YMEM = 0x20000; // allocate in Y memory bank

// note that attributes may combine several attribute bits.

extern const Attribute A_XMEM;
extern const Attribute A_YMEM;

class CGCStar : public CGStar {
private:
	int forkId;

protected:
	void setForkId() { forkId = TRUE; }

	// If "name" is a state, add the state to the list of referenced
	// states.  Then check to see whether name is a PortHole. If so,
	// get the reference from the geodesic.  Otherwise, invoke the
	// same method in CGStar.
	StringList getRef(const char* name);
	StringList getRef2(const char* name, const char* offset);

	// If "name" is a state, and is not already on the list
	// referencedStates, add it.
	void registerState(const char* name);

	// Add lines to be put at the beginning of the code file
	void addInclude(const char* decl);

	// Add declarations, to be put at the beginning of the main section
	void addDeclaration(const char* decl);

	// Add global declarations, to be put ahead of the main section
	void addGlobal(const char* decl);

	// Add global declarations, to be put ahead of the main section
	void addMainInit(const char* decl);

public:
	CGCStar(): forkId(0) {}

	// List of all states pointed to in the code.
	// This is public so that CGCTarget and other targets can access it.
	StateList referencedStates;

	// start method clears out referencedStates list
	void start();

	// my domain
	const char* domain() const;

	void fire();

	// class identification
	int isA(const char*) const;

	// fork star identification
	int amIFork() { return forkId; }

	// offset initialize
	void offsetInit();
};

#endif
