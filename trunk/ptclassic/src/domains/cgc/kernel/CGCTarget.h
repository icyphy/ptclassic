#ifndef _CGCTarget_h
#define _CGCTarget_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer: Soonhoi Ha and E. A. Lee

 Basic target for C code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "HLLTarget.h"
#include "StringState.h"
#include "IntState.h"

class CGCPortHole;
class EventHorizon;
class CGCStar;
class SDFScheduler;

class CGCTarget : public HLLTarget {
public:
	CGCTarget(const char* name, const char* starclass, const char* desc);
	Block* makeNew() const;
	void headerCode();
	void setup();
	void wrapup();
	/* virtual */ void beginIteration(int repetitions, int depth);
	/* virtual */ void endIteration(int repetitions, int depth);

	// name the offset-pointer of portholes
	StringList offsetName(const CGCPortHole* p); 

	// virtual method to generate compiling command
	virtual StringList compileLine(const char* fName);

	// make public this method
	StringList correctName(const NamedObj& p) 
		{return  sanitizedFullName(p); }
	StringList appendedName(const NamedObj& p, const char* s);

	// set the hostMachine name
	void setHostName(const char* s) { hostMachine = s; }
	const char* hostName() { return (const char*) hostMachine; }

	// compile and run the code
	int compileCode();
	int runCode();

	// redefine writeCode: default file is "code.c"
	void writeCode(const char* name = NULL);

	// static buffering option can be set by parent target
	void wantStaticBuffering() { staticBuffering = TRUE; }
	int useStaticBuffering() { return int(staticBuffering); }

	// incrementally add a star to the code
	/* virtual */ int incrementalAdd(CGStar* s);

	// incremental addition of a Galaxy code
	/* virtual */ int insertGalaxyCode(Galaxy* g, SDFScheduler* s);

protected:
	char *schedFileName;

	// code strings
	CodeStream globalDecls;
	CodeStream include;
	CodeStream mainDecls;
	CodeStream mainInit;
	CodeStream wormIn;
	CodeStream wormOut;
	CodeStream mainClose;

	// virtual function to initialize strings
	virtual void initCodeStrings();

	// buffer size determination
	int allocateMemory();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// redefine frameCode() method
	/* virtual */ void frameCode();

	// redefine compileRun() method to switch the code streams stars refer
	// to.
	/* virtual */ void compileRun(SDFScheduler*);

	// redefine, compile and run code
	/* virtual */ int wormLoadCode();

	// Redefine:
	// Add wormInputCode after iteration declaration and before the
	// iteration body, and wormOutputCode just before the closure of
	// the iteration.
	/* virtual */ void wormInputCode(PortHole&);
	/* virtual */ void wormOutputCode(PortHole&);

	// return a name that can be used as C identifier, derived from
	// the actual name
	StringList sanitizedFullName(const NamedObj &b) const;

	// states
	IntState staticBuffering;
	IntState doCompile;
	StringState hostMachine;
	StringState funcName;
	StringState compileCommand;
	StringState compileOptions;
	StringState linkOptions;
	StringState saveFileName;

	// give a unique name for a galaxy. 
	int galId;
	int curId;

	// define the data structure of the galaxy and star
	virtual void galDataStruct(Galaxy& galaxy, int level=0);
	virtual void starDataStruct(CGCStar* block, int level=0);

	// make a comment explaining the following code.
	StringList sectionComment(StringList s);

private:
	// assign the variable for each geodesic (connection).
	void setGeoNames(Galaxy& galaxy);

	// Update the copy-offset (for embedded portholes) after
	// run before wrap-up stage.
	StringList updateCopyOffset();

	// setup forkDests list for all Fork input portholes
	// This complete list is needed to decide the buffer size computation.
	void setupForkDests(Galaxy&);

	// check the hostMachine and set the localHost flag if the hostMachine
	// is the same as the /bin/hostname.
	void checkHostMachine();

	int localHost;
};

#endif
