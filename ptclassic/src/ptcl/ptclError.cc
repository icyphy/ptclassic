static const char file_id[] = "ptclError.cc";
/*******************************************************************
SCCS Version identification :
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
// if there is already a result, append to it.
	char* res = PTcl::activeInterp->result;
	StringList msg;
	if (res && *res)
		msg << res << "\n";
	else
		msg << "ERROR: ";
	msg << m1 << (m2 ? m2 : "") << (m3 ? m3 : "");
	char* msgc = msg.newCopy();
	Tcl_SetResult(PTcl::activeInterp, msgc, TCL_VOLATILE);
	LOG_DEL; delete msgc;
}

void
Error :: warn(cc* m1, cc* m2, cc* m3) {
	cerr << "Warning: ";
	p3(m1,m2,m3);
}

void
Error :: error (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList n_m1 = o.fullName();
	n_m1 << ":" << m1;
	error(n_m1,m2,m3);
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
