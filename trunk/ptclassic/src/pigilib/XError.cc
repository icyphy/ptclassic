/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 10/4/90
 Renamed to XError.cc, 4/15/91

 This is a replacement version of the methods for the Error class 
 in Ptolemy -- link this in instead of Error.cc to go through the
 PrintErr error message handler in pigi.

*******************************************************************/
extern "C" {
	void PrintErr(const char*);
	void PigiErrorMark(const char*);
	void FindClear();
}

#include <stream.h>
#include "Error.h"
#include "Scheduler.h"
#include "miscFuncs.h"
#include "NamedObj.h"

typedef const char cc;

static void outMsg(cc* obj, int warn, cc* m1, cc* m2, cc* m3) {
	const char* status = warn ? "warning: " : "";
	if (!m2) m2 = "";
	if (!m3) m3 = "";
	int l = strlen(status)+strlen(m1)+strlen(m2)+strlen(m3) + 1;
	if (obj) l += strlen(obj) + 2;
	char* buf = new char[l];
	if (obj)
		sprintf (buf, "%s%s: %s%s%s", status, obj, m1, m2, m3);
	else
		sprintf (buf, "%s%s%s%s", status, m1, m2, m3);
	PrintErr (buf);
	delete buf;
}

void
Error :: error(cc* m1, cc* m2, cc* m3) {
	outMsg(0, 0, m1, m2, m3);
}

void
Error :: warn(cc* m1, cc* m2, cc* m3) {
	outMsg(0, 1, m1, m2, m3);
}

void
Error :: error (NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList nam = o.readFullName();
	PigiErrorMark(nam);
	outMsg(nam, 0, m1, m2, m3);
}

void
Error :: warn (NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList nam = o.readFullName();
	PigiErrorMark(nam);
	outMsg(nam, 1, m1, m2, m3);
	FindClear();
}

void
Error :: abortRun (cc *m1, cc* m2, cc* m3) {
	outMsg (0, 0, m1, m2, m3);
	Scheduler::requestHalt();
}

void
Error :: abortRun (NamedObj& o, cc* m1, cc* m2, cc* m3) {
	error (o, m1, m2, m3);
	Scheduler::requestHalt();
}

// marking is supported in this implementation
int Error :: canMark() { return 1;}

void Error :: mark(NamedObj& o) {
	StringList n = o.readFullName();
	PigiErrorMark(n);
}
