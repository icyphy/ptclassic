#ifndef _CGStar_h
#define _CGStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee, J. Buck, J. Pino, T. M. Parks

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFStar.h"
#include "Fraction.h"
#include "CGConnect.h"
#include "Particle.h"
#include "DataStruct.h"
#include "Code.h"
#include "CGSymbol.h"
#include "IntState.h"

// the following function is provided by the SunOS and Ultrix libs.
// don't know how generally it is available.
extern "C" int strcasecmp(const char* s1, const char* s2);

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

	// get and set the procId
	int getProcId() { return int(procId); }
	void setProcId(int i) { procId = i; }

protected:

	// Process code, expanding macros.
	StringList processCode(CGCodeBlock&);
	StringList processCode(const char*);

	// Add processed code to the Target.
	void gencode(CGCodeBlock&);
	void gencode(const char*);

	// Add a string to the Target code.  (should be private?)
	void addCode(const char*);

	// Return the special character that introduces a macro
	// in a code block.  This character is used by gencode() to
	// parse the CodeBlock.  In this base class, the character is '$'.
	virtual char substChar() const;

	// For generation of error messages
	void codeblockError(const char* p1, const char* p2 = "");
	void macroError(const char* func, const StringList& argList);

	/* Expand macros that are defined for this star.
	   The following macros are recognized:

		$val(name)		Value of a state.
		$size(name)		Size of a state or port.
		$ref(name)		Reference to a state or port.
		$ref(name,offset)	Reference with offset.
		$label(name)		Unique label for codeblock.
		$codeblockSymbol(name)	Another name for label.
		$starSymbol(name)	Unique label for star.

	   The number, names, and meaning of
	   these functions can be easily redefined in derived classes.
	*/
	virtual StringList expandMacro(const char* func, const StringList& argList);

	// Match macro name and argument count.
	int matchMacro(const char* func, const StringList& argList, const char* name, int argc)
	{
	    return (strcasecmp(func, name) == 0) && (argList.size() == argc);
	}

	// Evaluate State used as MultiPortHole index.
	StringList expandPortName(const char*);

	// State or PortHole reference.
	virtual StringList expandRef(const char*);

	// State or PortHole reference with offset.
	virtual StringList expandRef(const char* name, const char* offset);

	// State value.
	virtual StringList expandVal(const char*);

	// Size of State or PortHole.
	StringList expandSize(const char*);

	// Update all PortHoles so that the offset is incremented by the
	// number of samples consumed or produced.
	void advance();

	// processor id 
	IntState procId;

private:
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
