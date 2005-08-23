// This may look like C code, but it is really -*- C++ -*-
#ifndef _ACSCGCTarget_h
#define _ACSCGCTarget_h

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


 Programmers:  Eric Pauer (Sanders), Christopher Hylands, Edward A. Lee
 Date of creation: 1/15/98
 Version: @(#)ACSCGCTarget.h	1.5 04/22/98

***********************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "ACSCGTarget.h"
#include "StringState.h"
#include "StringArrayState.h"
#include "IntState.h"
#include "HashTable.h"		// To pick up the definition of TextTable

// Defined in ACSDomain.cc
extern const char ACSdomainName[];


class ACSPortHole;
class ACSStar;
class SDFScheduler;

class ACSCGCTarget : public ACSCGTarget {
public:
    ACSCGCTarget(const char* name, const char* starclass, const char* desc,
            const char* category = NULL, const char* assocDomain = ACSdomainName);
    /* virtual */ Block* makeNew() const;
    
    ~ACSCGCTarget();

    // Class identification.
    /*virtual*/ int isA(const char*) const;

	/*virtual*/ int compileCode();
	/*virtual*/ int runCode();

	/*virtual*/ StringList comment(const char* text, const char* begin=NULL,
	    const char* end=NULL, const char* cont=NULL);

	/*virtual*/ void beginIteration(int repetitions, int depth);
	/*virtual*/ void endIteration(int repetitions, int depth);

	// virtual method to generate compiling command
	virtual StringList compileLine(const char* fName);

	// set the hostMachine name
	void setHostName(const char* s) { targetHost = s; }
	const char* hostName() { return (const char*) targetHost; }

	// redefine writeCode: default file is "code.c"
	/*virtual*/ void writeCode();

	// static buffering option can be set by parent target
	void wantStaticBuffering() { staticBuffering = TRUE; }
	int useStaticBuffering() { return int(staticBuffering); }

	// incrementally add a star to the code
	/*virtual*/ int incrementalAdd(CGStar* s, int flag = 1);

	// incremental addition of a Galaxy code
	/*virtual*/ int insertGalaxyCode(Galaxy* g, SDFScheduler* s);

	// have these methods for ACSCGCDDFTarget.
	// Later, ACSCGCDDFTarget will be merged into this ACSCGCTarget. Then,
	// remove these methods.
	CodeStream* removeStream(const char* n) { 
		CodeStream* cs = getStream(n);
		codeStringLists.remove(n);
		return cs;
	}
	void putStream(const char* n, CodeStream* cs) { addStream(n, cs); }
	
	// Functions defining the pragma mechanism. Currently used in
	// CGC to support command-line arguments in the generated code.
	/*virtual*/ StringList pragma () const {return "state_name_mapping STRING"; }
	/*virtual*/ StringList pragma (const char* parentname,	
				   const char* blockname) const;
	/*virtual*/ StringList pragma (const char* parentname,	
				   const char* blockname,	
				   const char* pragmaname) const;
	/*virtual*/ StringList pragma (const char* parentname,	
				   const char* blockname,	
				   const char* name,		
				   const char* value);		

protected:

	/*virtual*/ void setup();

	// code strings
	// "mainClose" is separated because when we insert a galaxy code
	// by insertGalaxyCode(), we need to put wrapup code at the end
	// of the body. 
	// "commInit" is separated to support recursion construct.
	// Some wormhole codes are put into several set of processor
	// groups. Since commInit code is inserted by pairSendReceive()
	// method of the parent target, we need to able to copy this code.
	// stream.

	CodeStream globalDecls;
	CodeStream include;
	CodeStream mainDecls;
	CodeStream mainInit;
	CodeStream commInit;
	CodeStream wormIn;
	CodeStream wormOut;
	CodeStream mainClose;

	// Four new CodeStreams to add in codes for command-line
	// argument support.
	CodeStream cmdargStruct;	
	CodeStream cmdargStructInit;	
	CodeStream setargFunc;		
	CodeStream setargFuncHelp;	

	CodeStream compileOptionsStream;
	CodeStream linkOptionsStream;
	CodeStream localLinkOptionsStream;
	CodeStream remoteLinkOptionsStream;
	CodeStream remoteFilesStream;
	CodeStream remoteCFilesStream;
	
	// virtual function to initialize strings
	virtual void initCodeStrings();

	// buffer size determination
	int allocateMemory();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// Stages of code generation.
	/*virtual*/ void headerCode();
        /*virtual*/ void mainLoopCode();
	/*virtual*/ void trailerCode();

	// Combine all sections of code;
	/*virtual*/ void frameCode();
    
	// redefine compileRun() method to switch the code streams stars refer
	// to.
	/*virtual*/ void compileRun(SDFScheduler*);

	// Redefine:
	// Add wormInputCode after iteration declaration and before the
	// iteration body, and wormOutputCode just before the closure of
	// the iteration.
	/*virtual*/ void wormInputCode(PortHole&);
	/*virtual*/ void wormOutputCode(PortHole&);

    	// Test if we need type conversion stars.
	/*virtual*/ int needsTypeConversionStar(PortHole&);

	// states
	IntState staticBuffering;
	StringState funcName;
	StringState compileCommand;
	StringState compileOptions;
	StringState linkOptions;
	StringArrayState resources;

	// combined linked options
	StringList getLinkOptions(int expandEnvironmentVars);

	// combined compile options
	StringList getCompileOptions(int expandEnvironmentVars);

	// complete list of C files on which the compilation depends
	StringList getDependentCFiles(int expandEnvironmentVars);

	// complete list of extra source files
	StringList getDependentSourceFiles(int expandEnvironmentVars);

	// if a remote compilation, then copy over dependent files
	// to the remote machine
	int processDependentFiles();

	// give a unique name for a galaxy. 
	int galId;
	int curId;

	// Generate declarations and initialization code for
	// Star PortHoles and States.
	virtual void declareGalaxy(Galaxy&);
	virtual void declareStar(ACSStar*);

	// splice copy stars or type conversion stars if necessary.
	void addSpliceStars();

        virtual CodeStream mainLoopBody();
	CodeStream mainLoop;

private:
	// setup forkDests list for all Fork input portholes
	// This complete list is needed to decide the buffer size computation.
	void setupForkDests(Galaxy&);

	// Use this to store the pragmas set for the target.
	TextTable mappings;

};

#endif //_ACSCGCTarget_h
