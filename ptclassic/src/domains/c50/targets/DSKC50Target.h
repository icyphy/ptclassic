#ifndef _DSKC50Target_h
#define _DSKC50Target_h 1

/******************************************************************
Version identification:
@(#)DSKC50Target.h	1.5	8/15/96

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

Programmer: A. Baensch
Date of creation: 30 June 1995

Target for TI TMSC50C5x DSP board.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "C50Target.h"
#include "StringState.h"
#include "IntState.h"

class DSKC50Target : public C50Target {
public:
    // Constructor
    DSKC50Target(const char* name, const char* desc,
		 const char* assocDomain = C50domainName);

    // Copy constructor
    DSKC50Target(const DSKC50Target&);

    /*virtual*/ int compileCode();
    /*virtual*/ void writeCode();
    /*virtual*/ int runCode();
    /*virtual*/ void frameCode();

    // Return a copy of itself
    /*virtual*/ Block* makeNew() const;

    // Type checking
    /*virtual*/ int isA(const char*) const;



protected:

    /*virtual*/ void headerCode();
    /*virtual*/ void trailerCode();
    
private:

    void initStates();

    CodeStream shellCmds;

    // flags

    int aicFlag;  // TRUE to initialize AIC
    int xmitFlag; // TRUE if a star will generate interrupt procedure for XINT
    int rcvFlag;  // TRUE if a star will generate interrupt procedure for RINT
    int timerFlag; //TRUE if a star will generate interrupt procedure for TINT


};

#endif










