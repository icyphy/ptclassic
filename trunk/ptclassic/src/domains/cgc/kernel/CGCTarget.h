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

class CGCTarget : public BaseCTarget {
public:
	CGCTarget();
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
protected:
	char *schedFileName;
	StringList staticDeclarations = "";
	StringList include = "";
	StringList mainDeclarations = "";
	StringList mainInitialization = "";
	StringList mainCode = "";
private:
	StringList sectionComment(const StringList s);
	int galDataStruct(Galaxy& galaxy, int level=0);
	int starDataStruct(Block& block, int level=0);
	void setGeoNames(Galaxy& galaxy);
};

#endif
