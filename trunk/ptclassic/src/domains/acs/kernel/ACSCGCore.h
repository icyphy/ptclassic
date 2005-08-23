// This may look like C code, but it is really -*- C++ -*-
#ifndef _ACSCGCore_h
#define _ACSCGCore_h

/**********************************************************************
Copyright (c) 1998 The Regents of the University of California.
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


 Programmers:  James A. Lundblad
 Date of creation: 3/18/98
 Version: @(#)ACSCGCore.h	1.6 09/08/99

The ACS CG Core class provides a base class for all derived CG Core category classes. May instance variables and methods are stolen from CGCStar.


***********************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "ACSCore.h"
#include "ACSPortHole.h"
#include "Attribute.h"

class ACSTarget;

class ACSCGCore : public ACSCore {
public:

	// constructor without Corona reference for use by
	// derrived core class default constructors.
	ACSCGCore(const char* category) : ACSCore(category) { }

	// add a splice star to the spliceClust list.  If atEnd
	// is true, append it to the end, otherwise prepend it.
	void addSpliceStar(ACSStar* s, int atEnd);

	// initialize method clears out referencedStates list
	void initialize();

	// run this star or spliceClust if there are any splice stars
	int run();

	// redefine the setTarget method in order to set the symbolic precision
	// of fix stars and portholes as soon as the symbol lists become fully
	// initialized
	/* virtual */ int setTarget(Target* t);

    	// Generate declaration, initialization and function codes for
    	// command-line arguments
    	/* virtual */ StringList cmdargStates(Attribute a=ANY);
    	/* virtual */ StringList cmdargStatesInits(Attribute a=ANY);
    	/* virtual */ StringList setargStates(Attribute a=ANY) = 0;	
    	/* virtual */ StringList setargStatesHelps(Attribute a=ANY);	

    	// Generate declarations and initialization code for PortHoles
    	/* virtual */ StringList declarePortHoles(Attribute a=ANY);
    	/* virtual */ StringList initCodePortHoles(Attribute a=ANY);

    	// Generate declarations and initialization code for States
    	/* virtual */ StringList declareStates(Attribute a=ANY);
    	/* virtual */ StringList initCodeStates(Attribute a=ANY);

	// We're now in CG core territory.
	/* virtual */ int isCG() const { return TRUE; }

	// JMS
	  /*virtual*/ int isA(const char*) const;

protected:

	// Get list of all states pointed to in the code.
	virtual StateList& getReferencedStates() { return referencedStates; }

	// Get list of spliced stars.
	virtual const SequentialList& getSpliceClust() const { return spliceClust; }

	// Add a State to the list of referenced States.
	void registerState(State*);

	// main routine.
	// virtual int runIt() = 0;

	// Handle additional macro expansions for fix support (currently, only
	// the $precision macro is recognized).
	/*virtual*/
	StringList expandMacro(const char* func, const StringList& argList);

	// Expand a $precision macro to a reference of a precision variable for
	// ports/states of type FIX/FIXARRAY with attribute A_VARPREC (it is an
	// error to ask for the precision of other ports or states).  The
	// variable is of type fix_prec, a structure containing the tags "len"
	// and "intb" that hold the actual precision of the associated fix.
	StringList expandFixPrecisionMacro(const char* label);

	// Expand $precision macro for ports or array with offset specification
	StringList expandFixPrecisionMacro(const char* label, const char* offset);

	// Virtual functions. Expand State or PortHole reference macros.
	// If "name" is a state, add it to the list of referenced states.
	StringList expandRef(const char* name);
	StringList expandRef(const char* name, const char* offset);

	// Expand State value macros. If "name" state takes a command-
	// line argumanent, add it to the list of referenced states.
	// Else return its value.
        StringList expandVal(const char* name);

	// After firing that star, we may need to move the input data between
	// shared buffers (for example, Spread/Collect) since these movements
	// are not visible from the user.
	void moveDataBetweenShared();

	// Language specific code stream method for above.
	virtual StringList addMoveDataBetweenShared( int thereEmbedded, int startEmbedded, int stopEmbedded, const char* farName, const char* geoName, int numXfer ) {
	if ( thereEmbedded || startEmbedded || stopEmbedded || farName || geoName || numXfer ) return StringList(""); return StringList(""); }
	// construct symbolic precision for state or port with given name
	Precision newSymbolicPrecision(int length,int intBits, const char* name);

	// Generate declarations for PortHoles and States.
	virtual StringList declareBuffer(const ACSPortHole*) = 0;
	virtual StringList declareOffset(const ACSPortHole*) = 0;
	virtual StringList declareState(const State*) = 0;

	// Generate declaration, initilization and function
	// codes for command-line settable states.
        virtual StringList cmdargState(const State*) = 0;
        virtual StringList cmdargStatesInit(const State*) = 0;
        virtual StringList setargState(const State*) = 0;
        virtual StringList setargStatesHelp(const State*) = 0;

	// Generate initialization code for PortHoles and States.
	virtual StringList initCodeBuffer(ACSPortHole*) = 0;
	virtual StringList initCodeOffset(const ACSPortHole*) = 0;
	virtual StringList initCodeState(const State*) = 0;

	virtual StringList portBufferIndex(const ACSPortHole*) = 0;
	virtual StringList portBufferIndex(const ACSPortHole*, const State*, const char*) = 0;

	virtual StringList arrayStateIndexRef(const State *, const char*) = 0;

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

	// This function checks whether "state" is to be set from a command-line
	// argument. If it is, returns the name to be specified on the command-
	// line. The function returns "" otherwise.
	StringList cmdArg(const State* state) const;
	int isCmdArg (const State* state) const;

private:

	// List of all states pointed to in the code.
	StateList	referencedStates;

	// form a cluster of this star and spliced stars
	// initially put myself in spliceClust.
	SequentialList spliceClust;


};

#endif // _ACSCGCore_h
