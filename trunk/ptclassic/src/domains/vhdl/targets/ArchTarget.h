#ifndef _ArchTarget_h
#define _ArchTarget_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1996 The Regents of the University of California.
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

#include "SimVSSTarget.h"
#include "VHDLStar.h"
#include "VHDLCompDecl.h"
#include "VHDLSignal.h"
#include "VHDLState.h"
#include "VHDLFiring.h"
#include "VHDLFiregroup.h"
#include "VHDLDependency.h"
#include "Attribute.h"
#include "VHDLToken.h"
#include "VHDLMux.h"
#include "VHDLReg.h"

class ArchTarget : public SimVSSTarget {
public:
	ArchTarget(const char*, const char*, const char*);
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

	// Add the clock to the clock list and generate code to toggle it.
	void toggleClock(const char*);

	// Add the clock to the clock list and generate code
	// to assert it with the given value.
	void assertClock(const char*, int);

	// Run the code.
	/*virtual*/ int runCode();

	// Register the State reference.
	/*virtual*/ void registerState(State*, const char*, int=-1, int=-1);

	// Register temporary variable reference.
	/*virtual*/ void registerTemp(const char*, const char*);

	// Register PortHole reference.
	/*virtual*/ void registerPortHole(VHDLPortHole*, const char*, int,
					  int=-1, const char* ="");

	// Return the assignment operators for States and PortHoles.
	/*virtual*/ const char* stateAssign() { return ":=" ; }
	/*virtual*/ const char* portAssign() { return ":=" ; }
//	/*virtual*/ const char* portAssign() { return "<=" ; }

protected:
	// Members to support dependency graph construction.
	VHDLFiringList masterFiringList;
	VHDLFiringList newFiringList;

	VHDLFiregroupList firegroupList;

	VHDLDependencyList dependencyList;
	VHDLTokenList tokenList;
	VHDLMuxList muxList;
	VHDLRegList regList;

	/*virtual*/ void begin();

	/*virtual*/ void setup();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// Stages of code generation.
	/*virtual*/ void headerCode();
	/*virtual*/ void trailerCode();

	// Combine all sections of code;
	/*virtual*/ void frameCode();

	// Method to write out com file for VSS if needed.
	/*virtual*/ void writeComFile();

	// virtual function to add additional codeStreams.
	/*virtual*/ void addCodeStreams();

	// virtual function to initialize codeStreams.
	/*virtual*/ void initCodeStreams();

	// Initialize VHDLObjLists.
	/*virtual*/ void initVHDLObjLists();

	// virtual function to initialize firing lists.
	virtual void initFiringLists();

	// Return the condition indicating if system clock generator is needed.
        virtual int systemClock() { return systemClockUsed; }

	// Method called by comm stars to place important code into structure.
	/*virtual*/ void registerComm(int, int, int, const char*);

private:
	// Pointer to keep track of current firing.
	VHDLFiring *currentFiring;

	// A list of clocks to trigger in order.
	StringList clockList;

	// Relevant code streams.
	CodeStream component_declarations;
	CodeStream signal_declarations;
	CodeStream component_mappings;
	CodeStream configuration_declaration;
	CodeStream preSynch;
	CodeStream firingAction;
	CodeStream postSynch;
	CodeStream ctlerAction;

	// General VHDLObjLists.
	VHDLPortList systemPortList;
	VHDLCompDeclList mainCompDeclList;
	VHDLSignalList mainSignalList;
	VHDLStateList stateList;

	// The following are for keeping track of components and
	// signals within one firing.
	VHDLGenericList firingGenericList;
	VHDLPortList firingPortList;
	VHDLSignalList firingSignalList;
	VHDLVariableList firingVariableList;

	// The following are for keeping track of components and
	// signals having to do with the controller.
	VHDLGenericList ctlerGenericList;
	VHDLPortList ctlerPortList;
	VHDLSignalList ctlerSignalList;
	VHDLVariableList ctlerVariableList;

	// Merge the Star's signal list with the Target's signal list.
	void mergeSignalList(VHDLSignalList*);

	// Connect a source of the given value to the given signal.
	void connectSource(StringList, VHDLSignal*);

	// Connect a multiplexor between the given input and output signals.
	void connectMultiplexor(StringList, VHDLSignalList*, VHDLSignal*, VHDLSignal*);

	// Connect a register between the given input and output signals.
	void connectRegister(VHDLSignal*, VHDLSignal*, VHDLSignal*);

	// Connect a clock generator driving the given signal.
	void connectClockGen(StringList);

	// Flag indicating if system clock generator is needed.
        int systemClockUsed;

	// Set the condition indicating system clock generator is needed.
	void setSystemClockUsed() { systemClockUsed = 1; }

	// Flag indicating if registers are needed.
        int regIntsUsed;
        int regRealsUsed;

	// Set the condition indicating registers are needed.
	void setRegisterInts() { regIntsUsed = 1; }
	void setRegisterReals() { regRealsUsed = 1; }

	// Return the condition indicating if registers are needed.
        int registerInts() { return regIntsUsed; }
        int registerReals() { return regRealsUsed; }

	// Flag indicating if multiplexors are needed.
        int muxIntsUsed;
        int muxRealsUsed;

	// Set the condition indicating multiplexors are needed.
	void setMultiplexorInts() { muxIntsUsed = 1; }
	void setMultiplexorReals() { muxRealsUsed = 1; }

	// Return the condition indicating if multiplexors are needed.
        int multiplexorInts() { return muxIntsUsed; }
        int multiplexorReals() { return muxRealsUsed; }

	// Flag indicating if sources are needed.
        int sorIntsUsed;
        int sorRealsUsed;

	// Set the condition indicating sources are needed.
	void setSourceInts() { sorIntsUsed = 1; }
	void setSourceReals() { sorRealsUsed = 1; }

	// Return the condition indicating if sources are needed.
        int sourceInts() { return sorIntsUsed; }
        int sourceReals() { return sorRealsUsed; }

	// Add in sensitivity list of input ports.
	// Do this explicitly for sake of synthesis.
	StringList addSensitivities(VHDLFiring*, int);

	// Add in wait statement with list of input ports.
	// Do this explicitly for sake of simulation.
	StringList addWaitStatement(VHDLFiring*, int);

	// Add in variable refs here from variableList.
	StringList addVariableRefs(VHDLFiring*, int);

	// Add in port to variable transfers here from portVarList.
	StringList addPortVarTransfers(VHDLFiring*, int);

	// Add in firing declarations here.
	StringList addDeclarations(VHDLFiring*, int);

	// Add in firing actions here.
	StringList addActions(VHDLFiring*, int);

	// Add in variable to port transfers here from varPortList.
	StringList addVarPortTransfers(VHDLFiring*, int);

	// Register compDecls and merge signals.
	void registerAndMerge(VHDLFiring*);

	// Generate the entity_declaration.
	void buildEntityDeclaration(int);

	// Generate the architecture_body_opener.
	void buildArchitectureBodyOpener(int);

	// Add in component declarations here from mainCompDeclList.
	void buildComponentDeclarations(int);

	// Generate the architecture_body_closer.
	void buildArchitectureBodyCloser(int);

	// Add in configuration declaration here from mainCompDeclList.
	void buildConfigurationDeclaration(int);

	// Generate the clock generator entity and architecture.
	StringList clockGenCode();

	// Generate the register entity and architecture.
	StringList regCode(StringList);

	// Generate the multiplexor entity and architecture.
	StringList muxCode(StringList);

	// Generate the source entity and architecture.
	StringList sourceCode(StringList);

//	// Merge all firings into one cluster.
//	void allFiringsOneCluster();

};

#endif
