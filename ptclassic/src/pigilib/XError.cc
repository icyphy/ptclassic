static const char file_id[] = "XError.cc";

/*******************************************************************
SCCS Version identification :
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
 Date of creation: 10/4/90
 Renamed to XError.cc, 4/15/91

 This is a replacement version of the methods for the Error class 
 in Ptolemy -- link this in instead of Error.cc to go through the
 PrintErr error message handler in pigi.

*******************************************************************/

// Do the right thing for sol2 boolean defs.  compat.h must be included
// first so sys/types.h is included correctly.
#include "sol2compat.h"

// Include standard include files to prevent conflict with
// the type definition Pointer used by "rpc.h". BLE
#include <stdio.h>
#include <string.h>
#include <stream.h>

#include "StringList.h"			/* define StringList */
#include "Error.h"			/* define Error class */
#include "Scheduler.h"			/* define Scheduler::requestHalt */
#include "miscFuncs.h"			/* define savestring */
#include "NamedObj.h"			/* define NamedObj class */
#include "PtGate.h"

#if defined(hppa)
/* Include math.h outside of extern "C" */
/* Otherwise, we get errors with pow() g++2.7.0 via vemInterface.h */
#include <math.h>
#endif

// Pigilib includes: vemInterface.h includes rpc.h
extern "C" {
#define Pointer screwed_Pointer         /* rpc.h and type.h define Pointer */
#include "vemInterface.h"		/* define PrintErr, PrintCon, ViXXX */
#include "exec.h"			/* define PigiErrorMark */
#include "ganttIfc.h"			/* define FindClear */
#include "xfunctions.h"			/* define win_msg */
#undef Pointer
}

// the gate object ensures that messages come out in one piece even
// with multi-threading.
static KeptGate gate;

typedef const char cc;

static void outMsg(cc* obj, int warn, cc* m1, cc* m2, cc* m3) {
	CriticalSection region(gate);
	StringList buf;
	buf << (warn ? "Warning: " : "Error: ");
	if ( obj ) buf << obj << ": ";
	if ( m1 ) buf << m1;
	if ( m2 ) buf << m2;
	if ( m3 ) buf << m3;
	PrintErrNoTag(buf);
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
Error :: error (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	char* objname = savestring(o.fullName());
	PigiErrorMark(objname);
	outMsg(objname, 0, m1, m2, m3);
	delete [] objname;
}

void
Error :: warn (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	char* objname = savestring(o.fullName());
	PigiErrorMark(objname);
	outMsg(objname, 1, m1, m2, m3);
	FindClear();
	delete [] objname;
}

void
Error :: abortRun (cc *m1, cc* m2, cc* m3) {
	outMsg (0, 0, m1, m2, m3);
	Scheduler::requestHalt();
}

void
Error :: abortRun (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	error (o, m1, m2, m3);
	Scheduler::requestHalt();
}

static void info(cc* obj, cc* m1, cc* m2, cc* m3) {
	CriticalSection region(gate);
	StringList buf;
	if ( obj ) buf << obj << ": ";
	if ( m1 ) buf << m1;
	if ( m2 ) buf << m2;
	if ( m3 ) buf << m3;
	if (ViGetErrWindows()) {
	  win_msg(buf);
	}
	else {
	  PrintCon(buf);
	}
}

void
Error :: message (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	char* objname = savestring(o.fullName());
	info(objname, m1, m2, m3);
	delete [] objname;
}

void
Error :: message (cc* m1, cc* m2, cc* m3) {
	info(0, m1, m2, m3);
}

// marking is supported in this implementation
int Error :: canMark() { return 1;}

void Error :: mark(const NamedObj& o) {
	char* objname = savestring(o.fullName());
	PigiErrorMark(objname);
	delete [] objname;
}
