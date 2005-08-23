static const char file_id[] = "Error.cc";
/*******************************************************************
SCCS Version identification :
@(#)Error.cc	2.13	02/06/97

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
 Date of creation: 4/14/91

 An error handling class.  This particular implementation assumes
 a text-based output (error messages appear on the standard error).
 An alternate implementation is provided with pigi to pop up error
 windows.

*******************************************************************/
#include "Error.h"
#include <stream.h>
#include "Scheduler.h"
#include "NamedObj.h"
#include "PtGate.h"

typedef const char cc;

static KeptGate gate;

static void p4(cc* m1, cc* m2, cc* m3=0, cc* m4=0) {
	CriticalSection region(gate);
	if (!m2) m2 = "";
	if (!m3) m3 = "";
	if (!m4) m4 = "";
	cerr << m1 << m2 << m3 << m4 << "\n";
}

void
Error :: error(cc* m1, cc* m2, cc* m3) {
	p4("ERROR: ",m1,m2,m3);
}

void
Error :: warn(cc* m1, cc* m2, cc* m3) {
	p4("Warning: ",m1,m2,m3);
}

void
Error :: error (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList n = "ERROR: ";
	n << o.fullName() << ": ";
	p4(n,m1,m2,m3);
}

void
Error :: warn (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList n = "Warning: ";
	n << o.fullName() << ": ";
	p4(n,m1,m2,m3);
}

void
Error :: message(cc* m1, cc* m2, cc* m3) {
	CriticalSection region(gate);
	p4(m1,m2,m3);
}

void
Error :: message (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList n = o.fullName();
	n << ": ";
	p4(n,m1,m2,m3);
}

void
Error :: abortRun (cc *m1, cc* m2, cc* m3) {
	error (m1, m2, m3);
	Scheduler::requestHalt();
}

void
Error :: abortRun (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	error (o, m1, m2, m3);
	Scheduler::requestHalt();
}

// marking is not supported in this implementation
int Error :: canMark() { return 0;}
void Error :: mark(const NamedObj&) {}
