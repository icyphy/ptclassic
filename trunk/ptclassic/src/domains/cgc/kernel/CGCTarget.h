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

 Programmer: Soonhoi Ha, E. A. Lee, and T. M. Parks

 Basic target for C code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "HLLTarget.h"
#include "StringState.h"
#include "StringArrayState.h"
#include "IntState.h"

class CGCPortHole;
class EventHorizon;
class CGCStar;
class SDFScheduler;

class CGCTarget : public HLLTarget {
public:
	CGCTarget(const char* name, const char* starclass, const char* desc);
	/*virtual*/ Block* makeNew() const;

	/*virtual*/ int compileCode();
	/*virtual*/ int runCode();

	/*virtual*/ StringList comment(const char*, const char*,
	    const char*, const char*);

	/*virtual*/ void beginIteration(int repetitions, int depth);
	/*virtual*/ void endIteration(int repetitions, int depth);

	// name the offset-pointer of portholes
	StringList offsetName(const CGCPortHole* p); 

	// virtual method to generate compiling command
	virtual StringList compileLine(const char* fName);

	// make public this method
	StringList correctName(const NamedObj& p) 
		{return  sanitizedFullName(p); }

	// set the hostMachine name
	void setHostName(const char* s) { targetHost = s; }
	const char* hostName() { return (const char*) targetHost; }

	// redefine writeCode: default file is "code.c"
	void writeCode();

	// static buffering option can be set by parent target
	void wantStaticBuffering() { staticBuffering = TRUE; }
	int useStaticBuffering() { return int(staticBuffering); }

	// incrementally add a star to the code
	/*virtual*/ int incrementalAdd(CGStar* s, int flag = 1);

	// incremental addition of a Galaxy code
	/*virtual*/ int insertGalaxyCode(Galaxy* g, SDFScheduler* s);

	// have these methods for CGCDDFTarget.
	// Later, CGCDDFTarget will be merged into this CGCTarget. Then,
	// remove these methods.
	CodeStream* removeStream(const char* n) { 
		CodeStream* cs = getStream(n);
		codeStringLists.remove(n);
		return cs;
	}
	void putStream(const char* n, CodeStream* cs) { addStream(n, cs); }
	
	// return TRUE if a function is being defined.
	int makingFunc() { return (getStream("mainInit") != &mainInit); }

protected:

	/*virtual*/ void setup();

	char *schedFileName;

	// code strings
	// "mainClose" is separated because when we insert a galaxy code
	// by insertGalaxyCode(), we need to put wrapup code at the end
	// of the body. 
	// "galStruct" is separated since we need to this
	// declaration inside a function call if we need to make a function
	// from a galaxy or a star while global variable is put outside 
	// of a function.
	// "commInit" is separated to support recursion construct.
	// Some wormhole codes are put into several set of processor
	// groups. Since commInit code is inserted by pairSendReceive()
	// method of the parent target, we need to able to copy this code.
	// stream.

	CodeStream globalDecls;
	CodeStream galStruct;
	CodeStream include;
	CodeStream mainDecls;
	CodeStream mainInit;
	CodeStream commInit;
	CodeStream wormIn;
	CodeStream wormOut;
	CodeStream mainClose;

	// virtual function to initialize strings
	virtual void initCodeStrings();

	// Splice in stars.
	/*virtual*/ int modifyGalaxy();

	// buffer size determination
	int allocateMemory();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// Initial stage of code generation.
	/*virtual*/ void headerCode();

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

	// return a name that can be used as C identifier, derived from
	// the actual name
	StringList sanitizedFullName(const NamedObj &b) const;

	StringList appendedName(const NamedObj& p, const char* s);

	// states
	IntState staticBuffering;
	StringState funcName;
	StringState compileCommand;
	StringState compileOptions;
	StringState linkOptions;
	StringArrayState resources;

	// give a unique name for a galaxy. 
	int galId;
	int curId;

	// define the data structure of the galaxy and star
	virtual void galDataStruct(Galaxy& galaxy, int level=0);
	virtual void starDataStruct(CGCStar* block, int level=0);

	// splice copy stars or type conversion stars if necessary.
	void addSpliceStars();

private:
	// setup forkDests list for all Fork input portholes
	// This complete list is needed to decide the buffer size computation.
	void setupForkDests(Galaxy&);
};

#endif
