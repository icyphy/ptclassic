#ifndef _VHDLTarget_h
#define _VHDLTarget_h 1
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

#include "HLLTarget.h"
#include "VHDLStar.h"
#include "VHDLFiring.h"
#include "VHDLVariable.h"
#include "VHDLArc.h"
#include "VHDLGeneric.h"
#include "VHDLPort.h"
#include "VHDLSignal.h"
#include "VHDLCompDecl.h"
#include "FloatArrayState.h"
#include "IntArrayState.h"
#include "ComplexArrayState.h"

// Defined in VHDLDomain.cc
extern const char VHDLdomainName[];

class VHDLTarget : public HLLTarget {
public:
	// Constructor
	VHDLTarget(const char* name, const char* starclass,
		   const char* desc, const char* assocDomain = VHDLdomainName);

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const;

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// Main routine.
	virtual int runIt(VHDLStar*);

	// Method called by comm stars to place important code into structure.
	virtual void registerComm(int, int, int, const char*) {};

	// Redefined from CGTarget to avoid cout messages.
	/*virtual*/ sendWormData(PortHole&);
	/*virtual*/ receiveWormData(PortHole&);

	// redefine writeCode: default file is "code.vhd"
	/*virtual*/ void writeCode();

	// Change all double underscores to single ones to ensure
	// proper identifiers.
	void singleUnderscore();

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

	// Register the constant storage reference.
	virtual void registerDefine(const char*, const char*, const char*);

	// Register the State reference.
	virtual void registerState(State*, const char*, int=-1, int=-1);

	// Register PortHole reference.
	virtual void registerPortHole(VHDLPortHole*, const char*, int,
				      int=-1, const char* ="");

	// Return the assignment operators for States and PortHoles.
	virtual const char* stateAssign() { return ":="; }
	virtual const char* portAssign() { return ":="; }

protected:
	CodeStream entity_declaration;
	CodeStream architecture_body_opener;
	CodeStream variable_declarations;
	CodeStream architecture_body_closer;
	CodeStream mainDecls;
	CodeStream loopOpener;
	CodeStream loopCloser;
	CodeStream useLibs;
	CodeStream sysWrapup;
	CodeStream simWrapup;

	// Keep this at the top of this file to please the compiler.
	// Add code to the beginning of a CodeStream instead of the end.
	void prepend(StringList, CodeStream&);

	// Return the VHDL type corresponding to the State type.
	const char* stateType(const State* st);

	/*virtual*/ void setup();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// Stages of code generation.
	/*virtual*/ void headerCode();
	/*virtual*/ void trailerCode();

	// Combine all sections of code;
	/*virtual*/ void frameCode();

	// Register a read or write to an arc and the offset.
	void registerArcRef(VHDLPortHole*, int);

	VHDLArcList arcList;

	// virtual function to add additional codeStreams.
	virtual void addCodeStreams();

	// virtual function to initialize codeStreams.
	virtual void initCodeStreams();

	// Initialize VHDLObjLists.
	virtual void initVHDLObjLists();

	// Various hashed strings for quick comparison.
        const char* hashINTEGER;
        const char* hashINT;
        const char* hashinteger;
        const char* hashint;
        const char* hashFIX;
        const char* hashfix;
        const char* hashFLOAT;
        const char* hashfloat;
        const char* hashREAL;
        const char* hashreal;
        const char* hashCOMPLEX;
        const char* hashcomplex;
        const char* hashCHARACTER;
        const char* hashIN;
        const char* hashOUT;

	// Return a set of comments based on firingList.
	StringList addFiringComments(VHDLFiringList*, int=0);

	// Return a generic clause based on genList.
	StringList addGenericRefs(VHDLGenericList*, int=0);

	// Return a port clause based on portList.
	StringList addPortRefs(VHDLPortList*, int=0);

	// Return signal declarations based on signalList.
	StringList addSignalDeclarations(VHDLSignalList*, int=0);

	// Return component declarations based on compDeclList.
	StringList addComponentDeclarations(VHDLCompDeclList*, int=0);

	// Return component mappings based on compDeclList.
	StringList addComponentMappings(VHDLCompDeclList*, int=0);

	// Return configuration declarations based on compDeclList.
	StringList addConfigurationDeclarations(VHDLCompDeclList*, int=0);

	// Return variable declarations based on variableList.
	StringList addVariableDecls(VHDLVariableList*, int=0, int=0);

	// Make this protected, not private, so it is still visible
	// to derived targets that want to use it.
	VHDLVariableList localVariableList;

	// Assign names for each geodesic according to port connections.
	void setGeoNames(Galaxy&);

private:
	VHDLVariableList variableList;

	// Return the VHDL type corresponding to the given const char*.
	const char* sanitizeType(const char*);

	// Clean up the code by wrapping around long lines as separate lines.
	void wrapAround(StringList*);

};

#endif
