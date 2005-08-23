/******************************************************************
Version identification:
@(#)Sub56Target.h	1.13 7/30/96

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

 Programmer: J. Pino

 Target for Motorola 56000 assembly code generation that generates
 two subroutines, ptolemyInit, ptolemyMain.
 
*******************************************************************/
#ifndef _Sub56Target_h
#define _Sub56Target_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CG56Target.h"
#include "StringState.h"
#include "IntState.h"

class Sub56Target : public CG56Target {
public:
	// Constructor
	Sub56Target(const char* name, const char* desc,
		    const char* assocDomain = CG56domainName);

	// Copy constructori
	Sub56Target(const Sub56Target&);

	/*virtual*/ void mainLoopCode();

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const;

	// Type checking
	/*virtual*/ int isA(const char*) const;

protected:
        /*virtual*/ void headerCode();

private:
	void initStates();
};

#endif
