#ifndef _CGCStar_h
#define _CGCStar_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

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
	int addInclude(const char* decl);

	// Add declarations, to be put at the beginning of the main section
	int addDeclaration(const char* decl, const char* name = NULL) {
		return addCode(decl, "mainDecls", name);
	}

	// Add global declarations, to be put ahead of the main section
	int addGlobal(const char* decl, const char* name = NULL) {
		return addCode(decl, "globalDecls", name);
	}

	// Add main initializations, to be put at the beginning of the main 
	// section. By giving the name, you can have only one initialization
	// routine among all star instances.
	int addMainInit(const char* decl, const char* name = NULL) {
		return addCode(decl, "mainInit", name);
	}

	// After each firing, update the offset pointers
	virtual void updateOffsets();

	// Before firing that star, we may need to move the input data from the
	// shared buffer to the private buffer in case of embedding: when the
	// input needs past samples or delays.
	void moveDataFromShared();

	// After firing that star, we may need to move the input data between
	// shared buffers (for example, Spread/Collect) since these movements
	// are not visible from the user.
	void moveDataBetweenShared();

	// If automatic type conversion is necessary, do it.
	void doTypeConversion();

	// get the actual buffer reference.
	virtual StringList getActualRef(CGCPortHole* p, const char* offset);

private:
	// define and initialize variables for C program.
	// Note that CGCTarget is a friend class to access these methods
	// freely.

	int emptyFlag;

	// declare PortHoles and States
	virtual void decideBufferType(CGCPortHole* p);
	virtual StringList declarePortHole(CGCPortHole* p);
	virtual StringList declareOffset(const CGCPortHole* p);
	virtual StringList declareState(const State* p);

	// initialize PortHoles
	virtual StringList initializeBuffer(CGCPortHole* p);
	virtual StringList initializeOffset(const CGCPortHole* p);

	// offset initialize
	void initBufPointer();
};

#endif
