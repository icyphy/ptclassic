#ifndef _CG56Target_h
#define _CG56Target_h 1
/******************************************************************
Version identification:
@(#)CG56Target.h	1.28	7/30/96

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

 Programmer: J. Buck and J. Pino

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "MotorolaTarget.h"
#include "ProcMemory.h"
#include "StringState.h"
#include "IntState.h"

extern StringList CG56ONE;

// Defined in CG56Domain.cc
extern const char CG56domainName[];

class CG56Target : public virtual MotorolaTarget {
public:
    // constructor
    CG56Target(const char* nam, const char* desc,
	       const char* assocDomain = CG56domainName);

    // copy constructor
    CG56Target(const CG56Target& src);

    // return a copy of itself
    /*virtual*/ Block* makeNew() const;

    /*virtual*/ int isA(const char*) const;
    /*virtual*/ void headerCode();
    /*virtual*/ const char* className() const;
    /*virtual*/ void setup();
    /*virtual*/ int compileCode();

protected:
    void initDataMembers();
    void writeFloat(double);
};

// Adds the galaxy parameter ONE.  This should be called by any multiprocessor
// target that contains a CG56 child target.
void addCG56One(Target *target, Galaxy *g);

#endif
