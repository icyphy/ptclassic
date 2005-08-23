#ifndef _HLLTarget_h
#define _HLLTarget_h 1
/******************************************************************
Version identification:
@(#)HLLTarget.h	1.14	7/30/96

Copyright (c) 1990-1996 The Regents of the University of California.
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
	HLLTarget(const char* nam, const char* startype, const char* desc,
		  const char* assocDomain = CGdomainName);

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const = 0;

	// Routines for writing code: schedulers may call these
	/*virtual*/ void beginIteration(int repetitions, int depth);
	/*virtual*/ void endIteration(int repetitions, int depth);

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// return a name that can be used as C++ identifiers, derived
	// from the actual name.
	StringList sanitize(const char* s) const;
	StringList sanitizedName(const NamedObj &b) const;
	virtual StringList sanitizedFullName(const NamedObj &b) const;
};

#endif
