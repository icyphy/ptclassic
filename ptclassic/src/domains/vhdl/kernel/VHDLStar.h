#ifndef _VHDLStar_h
#define _VHDLStar_h 1
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

 Programmer: Edward A. Lee, Michael C. Williamson

 This is the baseclass for stars that generate VHDL language code

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "VHDLPortHole.h"
#include "VHDLPort.h"
#include "VHDLGeneric.h"
#include "VHDLVariable.h"

class VHDLTarget;

class VHDLStar : public CGStar {
public:
	// List of all states used in the code.
	// This is public so that VHDLTarget and other targets can access it.
	StateList referencedStates;

	// Lists of all port and variable references in a firing.
	// This is public so that VHDLTarget and other targets can access it.
	StringList firingPortList;
	StringList firingVarList;

	// Lists of VHDL ports and variables referenced.
	// This is public so that VHDLTarget and other targets can access it.
	VHDLPortList firingPList;
	VHDLVariableList firingVList;

	// Add a State to the list of referenced States.
	void registerState(State*);

	// initialize method
	void initialize();

	// my domain
	const char* domain() const;

	// Sanitize a string so that it is usable as a VHDL identifier.
	const char* sanitize(const char*);

	// run this star
	int run();

	// class identification
	int isA(const char*) const;

	// Generate declarations for all PortHoles and States.
	StringList declarePortHoles();
	StringList declareStates();

	// Generate initialization code for all PortHoles and States.
	StringList initCodePortHoles();
	StringList initCodeStates();

protected:
	// access to target (cast is safe: always a VHDLTarget)
	VHDLTarget* targ() {
		return (VHDLTarget*)target();
	}

	// Expand macros that are defined for this star
        /* virtual */ StringList expandMacro(const char*, const StringList&);

	// Expand State or PortHole reference macros.
	/* virtual */ StringList expandRef(const char*);
	/* virtual */ StringList expandRef(const char*, const char*);
	// Form expression interspersing operators within arg list.
	StringList VHDLStar :: expandInterOp(const char*, const char*);

	// Assignment operator, depending on variable or signal
	StringList expandAssign(const char*);

private:
	// Generate declarations for individual PortHoles and States.
	StringList declareBuffer(const VHDLPortHole*);
	StringList declareOffset(const VHDLPortHole*);
	StringList declareState(const State*);

	// Generate initialization code for individual PortHoles and States.
	StringList initCodeBuffer(VHDLPortHole*);
	StringList initCodeOffset(const VHDLPortHole*);
	StringList initCodeState(const State*);

};

#endif
