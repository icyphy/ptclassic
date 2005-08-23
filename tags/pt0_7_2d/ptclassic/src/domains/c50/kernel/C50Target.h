#ifndef _C50Target_h
#define _C50Target_h 1
/******************************************************************
@Copyright (c) 1990-1996 The Regents of the University of California.
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

Programmer: Andreas Baensch
Date of Creation: 30 April 1995
Version: @(#)C50Target.h	1.7	8/18/96

Modeled after code by J. Buck and J. Pino.

Base target for TI 320C5x assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "TITarget.h"
#include "ProcMemory.h"
#include "StringState.h"
#include "IntState.h"

extern StringList C50ONE;

// Defined in C50Domain.cc
extern const char C50domainName[];

class C50Target : public virtual TITarget {
public:
	// constructor
	C50Target(const char* nam, const char* desc,
		  const char* assocDomain = C50domainName);

	// copy constructor
	C50Target(const C50Target& src);

	// return a new copy of itself
	/*virtual*/ Block* makeNew() const;

	/*virtual*/ int isA(const char*) const;
	void headerCode();
	const char* className() const;
	void setup();
	// default-C50 target should just generate code
	//int compileCode();

protected:
	void writeFloat(double);
	const char* memoryMapInitCode();
	const char* startCode();
};

// Adds the galaxy parameter ONE.  This should be called by any multiprocessor
// target that contains a C50 child target.
void addC50One(Target *target, Galaxy *g);

#endif
