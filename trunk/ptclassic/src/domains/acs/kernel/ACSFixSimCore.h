#ifndef _ACSFixSimCore_h
#define _ACSFixSimCore_h

/**********************************************************************
Copyright (c) 1998 The Regents of the University of California.
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


 Programmers:  J. A. Lundblad
 Date of creation: 3/11/98
 Version: @(#)ACSFixSimCore.h	1.6 09/08/99

***********************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "ACSCore.h"

// Global category string
extern const char ACSFixSimCategory[];

class ACSFixSimCore : public ACSCore {
public:

	// This is the default constructor for the Floating Point Simulation 
	// core category.
	ACSFixSimCore()  : ACSCore(ACSFixSimCategory) { } 

	// This is the flavor of constructor used by 
	// ACSCore* makeNew( ACSCorona & ).
	ACSFixSimCore(ACSCorona & corona_);

	// provides fixed-point support.
	/* virtual */ void wrapup();

	// tells target this is a fixed point core.
	/* virtual */ int isFixedPoint() const { return TRUE; }

	// JMS
	  /*virtual*/ int isA(const char*) const;

protected:

	// provides fixed-point support.
	/* virtual */ void setup();
	StringState OverflowHandler;
	IntState ReportOverflow;
	IntState RoundFix;

	int overflows, totalChecks;
	int checkOverflow (Fix& fix);


};




#endif // _ACSFixSimCore_h

