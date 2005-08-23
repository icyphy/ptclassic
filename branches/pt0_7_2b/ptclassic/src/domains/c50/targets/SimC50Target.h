#ifndef _SimC50Target_h
#define _SimC50Target_h 1

/******************************************************************
Version identification:
@(#)SimC50Target.h	1.2	05/26/98

@Copyright (c) 1998 The Regents of the University of California.
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

Programmer: Guner Arslan
Date of creation: 2 May 1998

Target for TI TMSC50C5x DSP board.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "C50Target.h"
#include "StringState.h"
#include "IntState.h"

class SimC50Target : public C50Target {
public:
    // Constructor
    SimC50Target(const char* name, const char* desc,
		 const char* assocDomain = C50domainName);

    // Copy constructor
    SimC50Target(const SimC50Target&);

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
    /*override TITarget*/void orgDirective(const char* memName, unsigned addr);
    /*override TITarget*/void writeFloat(double);
    /*override TITarget*/void writeFix(double);
    StringState typeOfBoard;

private:

    void initStates();

    CodeStream shellCmds;

    // flags

    int aicFlag;  // TRUE to initialize AIC
    int xmitFlag; // TRUE if a star will generate interrupt procedure for XINT
    int rcvFlag;  // TRUE if a star will generate interrupt procedure for RINT
    int timerFlag; //TRUE if a star will generate interrupt procedure for TINT
    int int0Flag; //TRUE if a star will generate interrupt procedure for INT0
    int int1Flag; //TRUE if a star will generate interrupt procedure for INT1
    int int2Flag; //TRUE if a star will generate interrupt procedure for INT2 
    int int3Flag; //TRUE if a star will generate interrupt procedure for INT3
    int int4Flag; //TRUE if a star will generate interrupt procedure for INT4
    int TDMRFlag; //TRUE if a star will generate interrupt procedure for TDM Receiver
    int TDMTFlag; //TRUE if a star will generate interrupt procedure for TDM Transmitter
    int TRAPFlag; //TRUE if a star will generate interrupt procedure for TRAP
    int NMIFlag; //TRUE if a star will generate interrupt procedure for NMI
};

#endif
