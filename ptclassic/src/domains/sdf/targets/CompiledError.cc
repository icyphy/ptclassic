static const char file_id[] = "CompiledError.cc";
/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck and E. A. Lee
 Date of creation: 12/7/91

 This replaces the standard error handler with one that does not
 interface to a scheduler.  It simply exits the process when abortRun()
 is called.

*******************************************************************/
#include "Error.h"
#include <stream.h>
#include "NamedObj.h"

typedef const char cc;

static void p3(cc* m1, cc* m2, cc* m3) {
	if (!m2) m2 = "";
	if (!m3) m3 = "";
	cerr << m1 << " " << m2 << " " << m3 << "\n";
}

void
Error :: error(cc* m1, cc* m2, cc* m3) {
	cerr << "ERROR: ";
	p3(m1,m2,m3);
}

void
Error :: warn(cc* m1, cc* m2, cc* m3) {
	cerr << "Warning: ";
	p3(m1,m2,m3);
}

void
Error :: error (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList n = o.fullName();
	cerr << "ERROR: " << n << ": ";
	p3(m1,m2,m3);
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
	exit(1);
}

void
Error :: abortRun (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	error (o, m1, m2, m3);
	exit(1);
}

// marking is not supported in this implementation
int Error :: canMark() { return 0;}
void Error :: mark(const NamedObj&) {}
