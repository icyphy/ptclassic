/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

Programmer: J. Pino

Target for Ariel S-56X DSP board.

*******************************************************************/
#ifndef _S56XTarget_h
#define _S56XTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CG56Target.h"
#include "StringState.h"
#include "IntState.h"

class S56XTarget : public CG56Target {
private:
    void initStates();
    CodeStream aioCmds;
    CodeStream shellCmds;
    StringState monitorProg;
protected:
        void headerCode();
        void trailerCode();
public:
	S56XTarget(const char*,const char*);
	S56XTarget(const S56XTarget&);
	int compileCode();
	void writeCode(const char* name);
	int runCode();
	Block* makeNew() const;
	int isA(const char*) const;
};

#endif
