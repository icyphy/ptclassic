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

#include "BaseCTarget.h"

class CGCPortHole;

class CGCTarget : public BaseCTarget {
public:
	CGCTarget(const char* name, const char* starclass, const char* desc);
	// copy constructor
	CGCTarget(const CGCTarget&);
	Block* clone() const;
	void headerCode();
	int setup(Galaxy&);
	int run();
	void wrapup();
	StringList beginIteration(int repetitions, int depth);

	// Method available to stars to add to lines that are
	// put at the beginning of the code file.
	void addInclude(const char* inc);

	// Method available to stars to add to declarations that are
	// put at the beginning of the main code segment.
	void addDeclaration(const char* decl);

	// Method available to stars to add to static declarations.
	void addGlobal(const char* decl);

	// Method available to stars to add to main initialization.
	void addMainInit(const char* decl);

	// name the offset of portholes
	StringList offsetName(CGCPortHole*);

	// make public this method
	StringList correctName(NamedObj& p) {return  sanitizedFullName(p); }

protected:
	char *schedFileName;
	StringList staticDeclarations;
	StringList include;
	StringList mainDeclarations;
	StringList mainInitialization;
	StringList mainCode;

	// buffer size determination
	int decideBufSize(Galaxy&);

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit(Galaxy&);

private:
	StringList includeFiles;
	StringList globalDecls;
	StringList sectionComment(const StringList s);
	int galDataStruct(Galaxy& galaxy, int level=0);
	int starDataStruct(Block& block, int level=0);
	void setGeoNames(Galaxy& galaxy);

	// setup forkDests list for all Fork input portholes
	// This complete list is needed to decide the buffer size computation.
	void setupForkDests(Galaxy&);
};

#endif
