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
#include "VHDLSignal.h"
#include "VHDLArc.h"
#include "VHDLState.h"
#include "VHDLPortVar.h"
#include "VHDLCluster.h"

class StructTarget : public VHDLTarget {
public:
	StructTarget(const char* name, const char* starclass, const char* desc);
	/*virtual*/ Block* makeNew() const;

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// The following are for keeping track of components and signals.
	VHDLPortList firingPortList;
	VHDLGenericList firingGenericList;
	VHDLPortMapList firingPortMapList;
	VHDLGenericMapList firingGenericMapList;
	VHDLSignalList firingSignalList;
	VHDLVariableList firingVariableList;
	VHDLPortVarList firingPortVarList;
	VHDLPortVarList firingVarPortList;

	VHDLPortList systemPortList;
	VHDLCompDeclList compDeclList;
	VHDLSignalList signalList;
	VHDLCompMapList compMapList;
	VHDLStateList stateList;
	VHDLArcList arcList;

	VHDLClusterList clusterList;

	// Main routine.
	/*virtual*/ int runIt(VHDLStar*);

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

	// Register component declaration.
	void registerCompDecl(StringList name,
			      VHDLPortList* portList,
			      VHDLGenericList* genList);

	// Merge the Star's signal list with the Target's signal list.
	void mergeSignalList(VHDLSignalList* starSignalList);

	// Register component mapping.
	void registerCompMap(StringList label, StringList name,
			     VHDLPortMapList* portMapList,
			     VHDLGenericMapList* genMapList);

	// Register the State reference.
	/*virtual*/ void registerState(State*, int=-1, int=-1);

	// Connect a source of the given value to the given signal.
	void connectSource(StringList, StringList);

	// Add a source component declaration.
	void registerSource(StringList);

	// Connect a selector between the given input and output signals.
	void connectSelector(StringList, StringList, StringList, StringList);

	// Add a selector component declaration.
	void registerSelector(StringList type);

	// Connect a register between the given input and output signals.
	void connectRegister(StringList, StringList, StringList);

	// Add a register component declaration.
	void registerRegister(StringList);

	// Register PortHole reference.
	/*virtual*/ void registerPortHole(VHDLPortHole*, int=-1);

  	// The only reason for redefining this from HLLTarget
 	// is to change the separator from "." to "_".
 	/*virtual*/ StringList sanitizedFullName(const NamedObj&) const;

	// Register a read or write to an arc and the offset.
	void registerArcRef(VHDLPortHole*, int);

protected:
	CodeStream component_declarations;
	CodeStream signal_declarations;
	CodeStream component_mappings;
	CodeStream configuration_declaration;
	CodeStream firingAction;

	// virtual function to add additional codeStreams.
	virtual void addCodeStreams();

	// virtual function to initialize codeStreams.
	virtual void initCodeStreams();

	// Initialize firing lists.
	void initFiringLists();

	/*virtual*/ void setup();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// Stages of code generation.
	/*virtual*/ void headerCode();
	/*virtual*/ void trailerCode();

	// Combine all sections of code;
	/*virtual*/ void frameCode();

private:
	// Flag indicating if registers are needed.
        int regsUsed;

	// Set the condition indicating registers are needed.
	void setRegisters() { regsUsed = 1; }

	// Return the condition indicating if registers are needed.
        int registers() { return regsUsed; }

	// Flag indicating if selectors are needed.
        int selsUsed;

	// Set the condition indicating selectors are needed.
	void setSelectors() { selsUsed = 1; }

	// Return the condition indicating if selectors are needed.
        int selectors() { return selsUsed; }

	// Flag indicating if sources are needed.
        int sorsUsed;

	// Set the condition indicating sources are needed.
	void setSources() { sorsUsed = 1; }

	// Return the condition indicating if sources are needed.
        int sources() { return sorsUsed; }
};

#endif
