#ifndef _StructTarget_h
#define _StructTarget_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
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
#include "VHDLState.h"
#include "VHDLPortVar.h"
#include "VHDLCluster.h"
#include "VHDLFiring.h"
#include "Attribute.h"

class StructTarget : public VHDLTarget {
public:
	StructTarget(const char* name, const char* starclass, const char* desc);
	/*virtual*/ Block* makeNew() const;

	// Class identification.
	/*virtual*/ int isA(const char*) const;

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

	// Register the State reference.
	/*virtual*/ void registerState(State*, const char*, int=-1, int=-1);

	// Register PortHole reference.
	/*virtual*/ void registerPortHole(VHDLPortHole*, const char*, int,
					  int=-1, const char* ="");

	// Return the assignment operators for States and PortHoles.
	/*virtual*/ const char* stateAssign();
	/*virtual*/ const char* portAssign();

  	// The only reason for redefining this from HLLTarget
 	// is to change the separator from "." to "_".
 	/*virtual*/ StringList sanitizedFullName(const NamedObj&) const;

protected:
	/*virtual*/ void setup();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// Stages of code generation.
	/*virtual*/ void headerCode();
	/*virtual*/ void trailerCode();

	// Combine all sections of code;
	/*virtual*/ void frameCode();

	// virtual function to add additional codeStreams.
	/*virtual*/ void addCodeStreams();

	// virtual function to initialize codeStreams.
	/*virtual*/ void initCodeStreams();

	// Initialize VHDLObjLists.
	/*virtual*/ void initVHDLObjLists();

	// virtual function to initialize firing lists.
	virtual void initFiringLists();

private:
	// A list of clocks to trigger in order.
	StringList clockList;

	// Relevant code streams.
	CodeStream component_declarations;
	CodeStream signal_declarations;
	CodeStream component_mappings;
	CodeStream configuration_declaration;
	CodeStream firingAction;
	CodeStream ctlerAction;

	// General VHDLObjLists.
	VHDLPortList systemPortList;
	VHDLCompDeclList compDeclList;
	VHDLSignalList mainSignalList;
	VHDLCompMapList mainCompMapList;
	VHDLStateList stateList;
	VHDLClusterList clusterList;

	// The following are for keeping track of components and
	// signals within one firing.
	VHDLPortList firingPortList;
	VHDLGenericList firingGenericList;
	VHDLPortMapList firingPortMapList;
	VHDLGenericMapList firingGenericMapList;
	VHDLSignalList firingSignalList;
	VHDLVariableList firingVariableList;
	VHDLPortVarList firingPortVarList;
	VHDLPortVarList firingVarPortList;

	// The following are for keeping track of components and
	// signals having to do with the controller.
	VHDLPortList ctlerPortList;
	VHDLGenericList ctlerGenericList;
	VHDLPortMapList ctlerPortMapList;
	VHDLGenericMapList ctlerGenericMapList;
	VHDLSignalList ctlerSignalList;
	VHDLVariableList ctlerVariableList;
	VHDLPortVarList ctlerPortVarList;
	VHDLPortVarList ctlerVarPortList;

	// Assign names for each geodesic according to port connections.
	void setGeoNames(Galaxy&);

	// Register component declaration.
	void registerCompDecl(StringList, VHDLPortList*, VHDLGenericList*);

	// Merge the Star's signal list with the Target's signal list.
	void mergeSignalList(VHDLSignalList*);

	// Register component mapping.
	void registerCompMap(StringList, StringList, VHDLPortMapList*,
			     VHDLGenericMapList*);

	// Connect a source of the given value to the given signal.
	void connectSource(StringList, StringList);

	// Add a source component declaration.
	void registerSource(StringList);

	// Connect a multiplexor between the given input and output signals.
	void connectMultiplexor(StringList, StringList, StringList,
				StringList);

	// Add a multiplexor component declaration.
	void registerMultiplexor(StringList type);

	// Connect a register between the given input and output signals.
	void connectRegister(StringList, StringList, StringList, StringList);

	// Add a register component declaration.
	void registerRegister(StringList);

	// Flag indicating if registers are needed.
        int regsUsed;

	// Set the condition indicating registers are needed.
	void setRegisters() { regsUsed = 1; }

	// Return the condition indicating if registers are needed.
        int registers() { return regsUsed; }

	// Flag indicating if multiplexors are needed.
        int muxsUsed;

	// Set the condition indicating multiplexors are needed.
	void setMultiplexors() { muxsUsed = 1; }

	// Return the condition indicating if multiplexors are needed.
        int multiplexors() { return muxsUsed; }

	// Flag indicating if sources are needed.
        int sorsUsed;

	// Set the condition indicating sources are needed.
	void setSources() { sorsUsed = 1; }

	// Return the condition indicating if sources are needed.
        int sources() { return sorsUsed; }

	// Add in sensitivity list of input ports.
	// Do this explicitly for sake of synthesis.
	void addSensitivities(VHDLCluster*, int);

	// Add in variable refs here from variableList.
	void addVariableRefs(VHDLCluster*, int);

	// Add in port to variable transfers here from portVarList.
	void addPortVarTransfers(VHDLCluster*, int);

	// Add in firing actions here.
	void addActions(VHDLCluster*, int);

	// Add in variable to port transfers here from varPortList.
	void addVarPortTransfers(VHDLCluster*, int);

	// Register compDecls and compMaps and merge signals.
	void registerAndMerge(VHDLCluster*);

	// Generate the entity_declaration.
	void buildEntityDeclaration(int);

	// Generate the architecture_body_opener.
	void buildArchitectureBodyOpener(int);

	// Add in component declarations here from compDeclList.
	void buildComponentDeclarations(int);

	// Generate the architecture_body_closer.
	void buildArchitectureBodyCloser(int);

	// Add in configuration declaration here from compDeclList.
	void buildConfigurationDeclaration(int);

	// Generate the register entity and architecture.
	StringList regCode();

	// Generate the multiplexor entity and architecture.
	StringList muxCode();

	// Generate the source entity and architecture.
	StringList sourceCode();

	// Merge all firings into one cluster.
	void allFiringsOneCluster();

};

#endif
