#ifndef _VHDLTarget_h
#define _VHDLTarget_h 1
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

#include "HLLTarget.h"
#include "VHDLStar.h"
#include "VHDLVariable.h"

class VHDLTarget : public HLLTarget {
public:
	VHDLTarget(const char* name, const char* starclass, const char* desc);
	/*virtual*/ Block* makeNew() const;

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// Main routine.
	virtual int runIt(VHDLStar*);

	// Redefined from CGTarget to avoid cout messages.
	/*virtual*/ sendWormData(PortHole&);
	/*virtual*/ receiveWormData(PortHole&);

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

	// Generate code for reading from a wormhole input port.
	/*virtual*/ void wormInputCode(PortHole&);

	// Generate code for writing to a wormhole output port.
	/*virtual*/ void wormOutputCode(PortHole&);

	// Register the temporary storage reference.
	virtual void registerTemp(const char*, const char*);

	// Register the State reference.
	virtual void registerState(State*, int=-1, int=-1);

	// Register PortHole reference.
	virtual void registerPortHole(VHDLPortHole*, int=-1);

	// Return the assignment operators for States and PortHoles.
	virtual const char* stateAssign();
	virtual const char* portAssign();

  	// The only reason for redefining this from HLLTarget
 	// is to change the separator from "." to "_".
 	virtual StringList sanitizedFullName(const NamedObj&) const;

protected:
	CodeStream entity_declaration;
	CodeStream architecture_body_opener;
	CodeStream variable_declarations;
	CodeStream architecture_body_closer;

	// Keep this at the top of this file to please the compiler.
	// Add code to the beginning of a CodeStream instead of the end.
	void prepend(StringList, CodeStream&);

	// Return the VHDL type corresponding to the State type.
	virtual StringList stateType(const State* st);

	/*virtual*/ void setup();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// Stages of code generation.
	/*virtual*/ void headerCode();
	/*virtual*/ void trailerCode();

	// Combine all sections of code;
	/*virtual*/ void frameCode();

private:
	// The following is for keeping track of variables.
	VHDLVariableList firingVariableList;
	VHDLVariableList variableList;

	// Assign names for each geodesic according to port connections.
	void setGeoNames(Galaxy&);

	// Return the VHDL type corresponding to the given const char*.
	virtual StringList sanitizeType(const char*);

	// Merge the Star's variable list with the Target's variable list.
	virtual void mergeVariableList(VHDLVariableList*);

	// virtual function to add additional codeStreams.
	virtual void addCodeStreams();

	// virtual function to initialize codeStreams.
	virtual void initCodeStreams();

	// Clean up the code by wrapping around long lines as separate lines.
	void wrapAround(StringList*);

};

#endif
