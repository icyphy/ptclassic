#ifndef _HLLTarget_h
#define _HLLTarget_h 1
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

#ifdef __GNUG__
#pragma interface
#endif

#include "CGTarget.h"
#include "SDFScheduler.h"
#include "StringState.h"
#include "IntState.h"

class HLLTarget : public CGTarget {
public:
	// constructor
	HLLTarget(const char* nam, const char* startype, const char* desc);
	void wrapup ();
	Block* makeNew() const = 0;

	// Routines for writing code: schedulers may call these
	void beginIteration(int repetitions, int depth);
	void endIteration(int repetitions, int depth);

protected:

	// return a name that can be used as C++ identifiers, derived
	// from the actual name.
	StringList sanitize(const char* s) const;
	StringList sanitizedName(const NamedObj &b) const;
	virtual StringList sanitizedFullName(const NamedObj &b) const;
};

#endif
