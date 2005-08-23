#ifndef _C50Target_h
#define _C50Target_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

Programmer: Andreas Baensch
Date of Creation: 30 April 1995

Modeled after code by J. Buck and J. Pino.

Base target for TI 320C5x  assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "TITarget.h"
#include "ProcMemory.h"
#include "StringState.h"
#include "IntState.h"

extern StringList C50ONE;

class C50Target : public virtual TITarget {
protected:
	void writeFloat(double);
public:
	C50Target (const char* nam, const char* desc) :
		TITarget(nam,desc,"C50Star") {}
	// copy constructor
	C50Target(const C50Target& src) : 
	  TITarget(src.name(),src.descriptor(),"C50Star") {}
	Block* makeNew() const;
	/*virtual*/ int isA(const char*) const;
	void headerCode();
	const char* className() const;
	void setup();
	int compileCode();
};

// Adds the galaxy parameter ONE.  This should be called by any multiprocessor
// target that contains a C50 child target.
void addC50One(Target *target, Galaxy *g);

#endif
