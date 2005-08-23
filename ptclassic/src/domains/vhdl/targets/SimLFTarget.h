#ifndef _SimLFTarget_h
#define _SimLFTarget_h 1
/******************************************************************
Version identification:
$Id$

 Programmers : Michael C. Williamson, Xavier Warzee (Thomson-CSF) 

 Target for VHDL code generation and interaction with Leapfrog simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLTarget.h"
#include "VHDLCompDecl.h"
#include "VHDLSignal.h"
#include "VHDLLFCSend.h"
#include "VHDLLFCReceive.h"
#include "CGCVSend.h"
#include "CGCVReceive.h"

class SimLFTarget : public VHDLTarget {
public:
	SimLFTarget(const char* name, const char* starclass,
		     const char* desc);
	/*virtual*/ Block* makeNew() const;

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// Routines to construct CG wormholes, using the
        // $PTOLEMY/src/domains/cgc/targets/main/CGWormTarget
	/*virtual*/ CommPair fromCGC(PortHole&);
	/*virtual*/ CommPair toCGC(PortHole&);

	// Initial stage of code generation.
	/*virtual*/ void headerCode();

	// Combine all sections of code.
	/*virtual*/ void frameCode();

	// redefine writeCode: default file is "code.vhd"
	/*virtual*/ void writeCode();

	// Compile, run the code.
	/*virtual*/ int compileCode();

	// Run the code.
	/*virtual*/ int runCode();

protected:
	// CodeStreams
	CodeStream cli_configs;

	// States.
	StringState cadence;
	StringState arch;
	StringState simarch;
	IntState analyze;
	IntState startup;
	IntState simulate;
	//IntState report;
	IntState interactive;
	
	/*virtual*/ void begin();

	/*virtual*/ void setup();

        // Method to write out com file for Leapfrog if needed.
        void setWriteCom();


	// virtual function to add additional codeStreams.
	/*virtual*/ void addCodeStreams();

	// Initialize codeStreams.
	/*virtual*/ void initCodeStreams();

	// Initialize VHDLObjLists.
	/*virtual*/ void initVHDLObjLists();

	// Method called by comm stars to place important code into structure.
	virtual void registerComm(int, int, int, const char*);

private:
	CodeStream top_uses;
	CodeStream top_entity;
	CodeStream top_architecture;
	CodeStream top_configuration;

	VHDLGenericList mainGenList;
	VHDLPortList mainPortList;
	VHDLSignalList topSignalList;
	VHDLCompDeclList topCompDeclList;

	int pairNumber;
	int writeCom;
	void configureCommPair(CommPair&);

        // Method to write out com file for Leapfrog if needed.
        void writeComFile();
};

#endif
