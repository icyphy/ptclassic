#ifndef _ACSFPCGCCore_h
#define _ACSFPCGCCore_h

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
 Date of creation: 4/1/98
 Version: @(#)ACSFPCGCCore.h	1.3 09/08/99

***********************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "ACSCGCCore.h"

// Global category string
extern const char ACSFPCGCCategory[];

class ACSFPCGCCore : public ACSCGCCore {
public:

	// This is the default constructor for the Floating Point Simulation 
	// core category.
	ACSFPCGCCore() : ACSCGCCore(ACSFPCGCCategory) { }

	// This is the flavor of constructor used by 
	// ACSCore* makeNew( ACSCorona & ).
	ACSFPCGCCore(ACSCorona & corona_) : ACSCGCCore(ACSFPCGCCategory) { }

	// JMS
	  /*virtual*/ int isA(const char*) const;

};




#endif // _ACSFPCGCCore_h

