#ifndef _CGStar_h
#define _CGStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

*******************************************************************/

#include "SDFStar.h"
#include "Fraction.h"
#include "CGConnect.h"
#include "Particle.h"
#include "DataStruct.h"
#include "Code.h"
#include "CGSymbol.h"

	////////////////////////////////////
	// class CGCodeBlock
	////////////////////////////////////

// In this generic architecture, a code block is identical to the base class
#define CGCodeBlock CodeBlock

class CGTarget;
class CGWormhole;

	////////////////////////////////////
	// class CGStar
	////////////////////////////////////

class CGStar : public SDFStar  {

public:
	// Constructor
	CGStar();

	// my domain
	const char* domain() const;

	// method for generating initial code
	virtual void initCode() {}

	// Generate code.  No data is grabbed or put onto geodesics.
	int fire();

	 // Pointer to target
	 CGTarget* myTarget() { return (CGTarget*)targetPtr; }

	// class identification
	int isA(const char*) const;

        // virtual method to return this pointer if it is a wormhole.
        // Return NULL if not.
        virtual CGWormhole* myWormhole();

        // max {cost of communication with its ancestors}
        int maxComm();

	Symbol codeblockSymbol;
	Symbol starSymbol;

protected:

	// Given a CodeBlock, generate output code. 
	// Macros referencing states and inputs and outputs are processed here.
	// The first method processes the codeblock and returns a StringList.
	// The second method adds the code the target's code using addCode.
	StringList processCode(CGCodeBlock& block);
	void gencode(CGCodeBlock& block);
	void addCode(const char*);

	// Return the special character that introduces a macro
	// in a code block.  This character is used by gencode() to
	// parse the CodeBlock.  In this base class, the character is '$'.
	virtual char substChar() const;

	// For generation of error messages
	void codeblockError(const char* p1, const char* p2 = "");

	// Process the macros that are defined for this star.
	// These are found by gencode.  In this class, the following
	// functions are handled:
	//      $val(name)      Value of state "name"
	//      $ref(name)      $mem(name):$addr(name)
	//      $ref2(name,o)   $mem(name):(address with offset)
	//      $label(name)    unique label for (codeblock,name) pair.
	//                      If the label does not exist, a new, unique
	//                      label is created.
	//	$codeblockSymbol(name)
	//			Another name for label
	//	$starSymbol(name)
	//			Unique symbol for (star, name) pair.
	// The number, names, and meaning of
	// these functions can be easily redefined in derived classes.
	virtual StringList processMacro(const char* func, const char* id,
				const char* arg2);

	// The following virtual methods return the substitution strings
	// for the ref macros.
	virtual StringList getRef(const char* name);
	virtual StringList getRef2(const char* name, const char* offset);

	// Look up the value of state and return it as a StringList.
	// A zero-length StringList is returned if there is no such State.
	virtual StringList lookupVal(const char* name);


	// Update all PortHoles so that the offset is incremented by the
	// number of samples consumed or produced.
	void advance();

private:
	// List of all local star labels
/*	StringList codeblockSymbols;
	StringList starSymbols;*/

	// Reset local codeblock labels
	void resetCodeblockSyms(){ codeblockSymbol.initialize(); }
};

/*
class CGStarPortIter : public BlockPortIter {
public:
	CGStarPortIter(CGStar& s) : BlockPortIter(s) {}
	CGPortHole* next() { return (CGPortHole*)BlockPortIter::next();}
	CGPortHole* operator++() { return next();}
};
*/

#endif
