#ifndef _CGCStar_h
#define _CGCStar_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: S. Ha, E. A. Lee, J. T. Buck, and T. M. parks

 This is the baseclass for stars that generate C language code

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif


#include "CGStar.h"
#include "CGCPortHole.h"
#include "Attribute.h"

class CGCTarget;

// New attribute bit definition
const bitWord AB_VARPREC  = 0x40;

const Attribute ANY = {0,0};

// New attribute for ports and states of type FIX/FIXARRAY.
// These attributes are ignored for ports/states of other types.

// fixed point precision may change at runtime;
// declare a precision variable holding the actual precision
const Attribute A_VARPREC   = {AB_VARPREC,0};
// fixed point precision does not change at runtime (the default)
const Attribute A_CONSTPREC = {0,AB_VARPREC};


class CGCStar : public CGStar {
public:
	// List of all states pointed to in the code.
	// This is public so that CGCTarget and other targets can access it.
	StateList referencedStates;

	// initialize method clears out referencedStates list
	void initialize();

	// my domain
	const char* domain() const;

	// run this star or spliceClust if there are any splice stars
	int run();

	// redefine the setTarget method in order to set the symbolic precision
	// of fix stars and portholes as soon as the symbol lists become fully
	// initialized
	/* virtual */ int setTarget(Target* t);

	// class identification
	int isA(const char*) const;

	// Generate declarations and initialization code for PortHoles
	StringList declarePortHoles(Attribute a=ANY);
	StringList initCodePortHoles(Attribute a=ANY);

	// Generate declarations and initialization code for States
	StringList declareStates(Attribute a=ANY);
	StringList initCodeStates(Attribute a=ANY);

	// add a splice star to the spliceClust list.  If atEnd
	// is true, append it to the end, otherwise prepend it.
	void addSpliceStar(CGCStar* s, int atEnd);

	// unfortunate, but we need to make special treatment for
	// Spread/Collect stars when splicing. Note that Spread/Collect
	// are not regular stars
	// If Spread, redefine to return -1, if Collect, return 1, otherwise 0.
	virtual int amISpreadCollect() {return 0; }

protected:
	// main routine.
	int runIt();

	// access to target (cast is safe: always a CGCTarget)
	CGCTarget* targ() {
		return (CGCTarget*)target();
	}

	// Handle additional macro expansions for fix support (currently, only
	// the $precision macro is recognized).
	/*virtual*/
	StringList expandMacro(const char* func, const StringList& argList);

	// Expand a $precision macro to a reference of a precision variable for
	// ports/states of type FIX/FIXARRAY with attribute A_VARPREC (it is an
	// error to ask for the precision of other ports or states).
	// The variable is of type fix_prec, a structure containing the tags "len"
	// and "intb" that hold the actual precision of the associated fix.
	StringList expandFixPrecisionMacro(const char* label);
	// for ports or array with offset specification
	StringList expandFixPrecisionMacro(const char* label, const char* offset);

	// Virtual functions. Expand State or PortHole reference macros.
	// If "name" is a state, add it to the list of referenced states.
	StringList expandRef(const char* name);
	StringList expandRef(const char* name, const char* offset);

	// Add a State to the list of referenced States.
	void registerState(State*);

	// Add lines to be put at the beginning of the code file
	int addInclude(const char* decl);

	// Add declarations, to be put at the beginning of the main section
	int addDeclaration(const char* decl, const char* name = NULL) {
		if (!name) name = decl;
		return addCode(decl, "mainDecls", name);
	}

	// Add global declarations, to be put ahead of the main section
	int addGlobal(const char* decl, const char* name = NULL) {
		if (!name) name = decl;
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

	// After firing that star, we may need to move the input data between
	// shared buffers (for example, Spread/Collect) since these movements
	// are not visible from the user.
	void moveDataBetweenShared();

	// construct symbolic precision for state or port with given name
	Precision newSymbolicPrecision(int length,int intBits, const char* name);

private:
	// Generate declarations for PortHoles and States.
	StringList declareBuffer(const CGCPortHole*);
	StringList declareOffset(const CGCPortHole*);
	StringList declareState(const State*);

	// Generate initialization code for PortHoles and States.
	StringList initCodeBuffer(CGCPortHole*);
	StringList initCodeOffset(const CGCPortHole*);
	StringList initCodeState(const State*);

	// form a cluster of this star and spliced stars
	// initially put myself in spliceClust.
	SequentialList spliceClust;
};

#endif
