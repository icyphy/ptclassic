#ifndef _BaseCTarget_h
#define _BaseCTarget_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 6/3/92

This Target serves as a base class for C++ and C code generation
targets, combining the features they have in common.

*******************************************************************/

#include "CGTarget.h"
#include "SDFScheduler.h"
#include "StringState.h"
#include "IntState.h"
#include "UserOutput.h"

class BaseCTarget : public CGTarget {
public:
	// constructor
	BaseCTarget(const char* nam, const char* startype, const char* desc);
	void start();
	void wrapup ();
	Block* clone() const = 0;

	// Routines for writing code: schedulers may call these
	StringList beginIteration(int repetitions, int depth);
	StringList endIteration(int repetitions, int depth);

	~BaseCTarget() { delSched();}
protected:
	StringState destDirectory;
	IntState loopScheduler;
	char *schedFileName;

	// Counter used to generate unique identifiers
	int unique;

	// return a name that can be used as C++ identifiers, derived
	// from the actual name.
	StringList sanitize(const char* s) const;
	StringList sanitizedName(const NamedObj &b) const;
	StringList sanitizedFullName(const NamedObj &b) const;
};

#endif
