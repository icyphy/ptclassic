#ifndef _CGCDDFCode_h
#define  _CGCDDFCode_h 1

/******************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

 Programmer: S. Ha

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGDDFCode.h"
#include "CodeStream.h"
#include "StringList.h"

class CGCDDFCode : public CGDDFCode {
public:
	CGCDDFCode(MultiTarget* t) : CGDDFCode(t) {}

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
