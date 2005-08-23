#ifndef _CGBDFTarget_h
#define _CGBDFTarget_h 1
/******************************************************************
Version identification:
@(#)CGBDFTarget.h	1.4	07/30/96

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer: J. Buck

BDF code generation!

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif
#include "CGTarget.h"

class CGBDFTarget : public CGTarget {
public:
	CGBDFTarget(const char* name, const char* starclass, const char* desc,
		    const char* assocDomain = CGdomainName);

	/* virtual */ void setup();

	// Return a copy of itself
	/* virtual */ Block* makeNew() const;

	// code to generate if/then/else constructs
	/* virtual */ void beginIf(PortHole& cond,int truthdir,
				  int depth,int haveElsePart);
	/* virtual */ void beginElse(int depth);
	/* virtual */ void endIf(int depth);

	// Code to generate while loops
	/* virtual */ void beginDoWhile(int depth);
	/* virtual */ void endDoWhile(PortHole& cond,int truthdir,int depth);
};

#endif
