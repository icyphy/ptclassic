#ifndef _CGCDDFTarget_h
#define  _CGCDDFTarget_h 1

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

 Programmer: S. Ha

 The DDF target for CGC domain.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGDDFTarget.h"

class CGCDDFTarget : public CGDDFTarget {
public:
	CGCDDFTarget(const char* name, const char* starclass, const char* desc);

	Block* makeNew() const;
	int isA(const char*) const;

///////// target dependent section ////////////////

	// for Case construct
	void startCode_Case(CGStar*,Geodesic*,CGTarget*);
	void middleCode_Case(int, CGTarget*);
	void endCode_Case(CGTarget*);

	// for DoWhile construct
	void startCode_DoWhile(Geodesic*, Geodesic*, CGTarget*);
	void endCode_DoWhile(CGStar*, Geodesic*, CGTarget*);

	// for For construct
	void startCode_For(CGStar*, Geodesic*, CGTarget*);
	void middleCode_For(CGStar*, CGStar*, int, int, CGTarget*);
	void endCode_For(CGTarget*);

/*
	// for Recur construct
	void startCode_Recur(int,int,CGTarget*);
	void middleCode_Recur(int, CGTarget*);
	void endCode_Recur(CGTarget*);
*/
};

#endif
