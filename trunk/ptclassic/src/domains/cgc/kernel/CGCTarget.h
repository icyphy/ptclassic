#ifndef _CGCTarget_h
#define _CGCTarget_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

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

class CGCTarget : public HLLTarget {
public:
	CGCTarget(const char* name, const char* starclass, const char* desc);
	Block* makeNew() const;
	void headerCode();
	void setup();
	void wrapup();
	void beginIteration(int repetitions, int depth);
	void endIteration(int repetitions, int depth);

	// Method available to stars to add to lines that are
	// put at the beginning of the code file.
	void addInclude(const char* inc);

	// Method available to stars to add to declarations that are
	// put at the beginning of the main code segment.
	void addDeclaration(const char* decl);

	// Method available to stars to add to static declarations.
	void addGlobal(const char* decl);

	// Method available to stars to add to procedures.
	void addProcedure(const char* decl);

	// Method available to stars to add to main initialization.
	void addMainInit(const char* decl);

	// name the offset of portholes
	StringList offsetName(const CGCPortHole* p); 

	// make public this method
	StringList correctName(const NamedObj& p) 
		{return  sanitizedFullName(p); }
	StringList appendedName(const NamedObj& p, const char* s);

	// compile and run the code
	int compileCode();
	int runCode();

	// generate code for a single processor in a multiprocessor target
	StringList generateCode();

	// static buffering option can be set by parent target
	void wantStaticBuffering() { staticBuffering = TRUE; }
	int useStaticBuffering() { return int(staticBuffering); }

protected:
	char *schedFileName;
	StringList staticDeclarations;
	StringList procedures;
	StringList include;
	StringList mainDeclarations;
	StringList mainInitialization;
	StringList wormIn;
	StringList wormOut;

	// buffer size determination
	int allocateMemory();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();

	// redefine frameCode() method
	void frameCode();

	// Redefine:
	// Add wormInputCode after iteration declaration and before the
	// iteration body, and wormOutputCode just before the closure of
	// the iteration.
	virtual void wormInputCode(PortHole&);
	virtual void wormOutputCode(PortHole&);

	// return a name that can be used as C identifier, derived from
	// the actual name
	StringList sanitizedFullName(const NamedObj &b) const;

	// states
	IntState staticBuffering;
	StringState funcName;
	StringState compileCommand;
	StringState compileOptions;
	StringState linkOptions;
	StringState saveFileName;

	// give a unique name for a galaxy. 
	int galId;

private:
	StringList includeFiles;
	StringList globalDecls;
	StringList sectionComment(const StringList s);
	virtual int galDataStruct(Galaxy& galaxy, int level=0);
	virtual int starDataStruct(CGCStar* block, int level=0);
	void setGeoNames(Galaxy& galaxy);

	// Update the copy-offset (for embedded portholes) after
	// run before wrap-up stage.
	StringList updateCopyOffset();

	// setup forkDests list for all Fork input portholes
	// This complete list is needed to decide the buffer size computation.
	void setupForkDests(Galaxy&);
};

#endif
