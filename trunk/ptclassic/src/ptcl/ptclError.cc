static const char file_id[] = "ptclError.cc";
/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 3/5/92

Implementation of the Error class that reports errors to Tcl.
warn and message still print to stderr as before.  The other
functions use Tcl to report errors, unless there is no active
Tcl interpreter.

*******************************************************************/
#include "PTcl.h"
#include "Error.h"
#include "SimControl.h"
#include "NamedObj.h"
#include <stream.h>
#include <stdio.h>
#include "miscFuncs.h"

typedef const char cc;

static void p3(cc* m1, cc* m2, cc* m3) {
	if (!m2) m2 = "";
	if (!m3) m3 = "";
	cerr << m1 << " " << m2 << " " << m3 << "\n";
}

void
Error :: error(cc* m1, cc* m2, cc* m3) {
	if (PTcl::activeInterp == 0) {
		cerr << "ERROR: ";
		p3(m1,m2,m3);
		return;
	}
	char* res = PTcl::activeInterp->result;
	StringList msg;
	if (res && *res)
		msg << res << "\n";
	else
		msg << "ERROR: ";
	msg << m1 << (m2 ? m2 : "") << (m3 ? m3 : "");
	Tcl_SetResult(PTcl::activeInterp, msg, TCL_VOLATILE);
}

void
Error :: warn(cc* m1, cc* m2, cc* m3) {
	cerr << "Warning: ";
	p3(m1,m2,m3);
}

void
Error :: error (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	if (PTcl::activeInterp == 0) {
		cerr << "ERROR: ";
		Error::message(o,m1,m2,m3);
		return;
	}
	if (!m2) m2 = "";
	if (!m3) m3 = "";
	StringList s = o.fullName();
	const char* name = s;
	int l = strlen(name) + strlen(m1) + strlen(m2) + strlen(m3) + 10;
	LOG_NEW; char* msg = new char[l];
	sprintf (msg, "ERROR: %s: %s%s%s", name, m1, m2, m3);
	Tcl_SetResult(PTcl::activeInterp, msg, TCL_VOLATILE);
	LOG_DEL; delete msg;
}

void
Error :: warn (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList n = o.fullName();
	cerr << "Warning: " << n << ": ";
	p3(m1,m2,m3);
}

void
Error :: message(cc* m1, cc* m2, cc* m3) {
	p3(m1,m2,m3);
}

void
Error :: message (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList n = o.fullName();
	cerr << n << ": ";
	p3(m1,m2,m3);
}

void
Error :: abortRun (cc *m1, cc* m2, cc* m3) {
	error (m1, m2, m3);
	SimControl::requestHalt();
}

void
Error :: abortRun (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	error (o, m1, m2, m3);
	SimControl::requestHalt();
}

// marking is not supported in this implementation
int Error :: canMark() { return 0;}
void Error :: mark(const NamedObj&) {}
