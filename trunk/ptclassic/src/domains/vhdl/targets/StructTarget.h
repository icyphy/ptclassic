#ifndef _StructTarget_h
#define _StructTarget_h 1
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

 Programmer: Michael C. Williamson

 Basic target for VHDL code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLTarget.h"
#include "VHDLStar.h"
#include "VHDLCompDecl.h"
#include "VHDLCompMap.h"

class StructTarget : public VHDLTarget {
public:
	StructTarget(const char* name, const char* starclass, const char* desc);
	/*virtual*/ Block* makeNew() const;

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// The following are for keeping track of components.
	VHDLCompDeclList compDeclList;
	VHDLCompMapList compMapList;

	// Main routine.
	virtual int runIt(VHDLStar*);

	// redefine writeCode: default file is "code.vhd"
	/*virtual*/ void writeCode();

	// Compile, run the code.
	/*virtual*/ int compileCode();

	// Generate a comment.
	/*virtual*/ StringList comment(const char* text,
				       const char* begin=NULL,
				       const char* end=NULL,
				       const char* cont=NULL);

	// Generate code to begin an iterative procedure.
	/*virtual*/ void beginIteration(int, int);

	// Generate code to end an iterative procedure.
	/*virtual*/ void endIteration(int, int);

	// Run the code.
	/*virtual*/ int runCode();

	// Assign names for each geodesic according to port connections.
	void setGeoNames(Galaxy&);

	// Declare PortHole buffer.
	StringList declBuffer(const VHDLPortHole*);

	// Declare State variable.
	StringList declState(const State*, const char*);

	// Register component declaration.
	void registerCompDecl(StringList name,
			      VHDLPortList* portList,
			      VHDLGenericList* genList);

	// Register component mapping.
	void registerCompMap(StringList label, StringList name,
			     VHDLPortMapList* portMapList,
			     VHDLGenericMapList* genMapList);

protected:
	CodeStream component_declarations;
	CodeStream signal_declarations;
	CodeStream component_mappings;
	CodeStream starDecls;
	CodeStream starInit;
	CodeStream firingAction;

	// virtual function to add additional codeStreams.
	virtual void addCodeStreams();

	// virtual function to initialize codeStreams.
	virtual void initCodeStreams();

	/*virtual*/ void setup();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// Stages of code generation.
	/*virtual*/ void headerCode();
	/*virtual*/ void trailerCode();

	// Combine all sections of code;
	/*virtual*/ void frameCode();

	// Generate declarations and initialization code for
	// Star PortHoles and States.
/*
	virtual void declareGalaxy(Galaxy&);
*/
	virtual void declareStar(VHDLStar*);

  	// The only reason for redefining this from HLLTarget
 	// is to change the separator from "." to "_".
 	/*virtual*/ StringList sanitizedFullName(const NamedObj&) const;

private:
};

#endif
