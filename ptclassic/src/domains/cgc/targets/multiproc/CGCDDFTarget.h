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

class CGCTarget;

class CGCDDFTarget : public CGDDFTarget {
public:
	CGCDDFTarget(const char* name, const char* starclass, const char* desc);

	Block* makeNew() const;
	int isA(const char*) const;

///////// target dependent section ////////////////
	// For "For" and "Recur" construct, we need to arrange the target
	// dependent codes for UnixSend, UnixReceive stars.
	/* virtual */ void prepCode(MultiTarget*, Profile*, int, int);
	// Signal that replication may occur or finish depending on the flag.
	/* virtual */ void signalCopy(int);

	// for Case construct
	/* virtual */ void startCode_Case(CGStar*,Geodesic*,CGTarget*);
	/* virtual */ void middleCode_Case(int, CGTarget*);
	/* virtual */ void endCode_Case(CGTarget*);

	// for DoWhile construct
	/* virtual */ void startCode_DoWhile(Geodesic*, Geodesic*, CGTarget*);
	/* virtual */ void endCode_DoWhile(CGStar*, Geodesic*, CGTarget*);

	// for For construct
	/* virtual */ void startCode_For(CGStar*, Geodesic*, CGTarget*);
	/* virtual */ void 
		middleCode_For(CGStar*, CGStar*, int, int, CGTarget*);
	/* virtual */ void endCode_For(CGTarget*);

	// for Recur construct
	/* virtual */ void 
		startCode_Recur(Geodesic*, PortHole*, const char*, CGTarget*);
	/* virtual */ void 
		middleCode_Recur(Geodesic*, Geodesic*, const char*, CGTarget*);
	/* virtual */ void endCode_Recur(Geodesic*, const char*, CGTarget*);

private:
	// No nested recursion is supported here. To allow nested recursion,
	// We need stack of these.

	// save code stream for recursion construct.
	CodeStream* saveGalStruct;
	CodeStream* saveMainInit;
	CodeStream* saveMainDecls;
	CodeStream* saveMainClose;
	CodeStream* saveCode;

	// recursive function definition.
	StringList funcName;
};

#endif
