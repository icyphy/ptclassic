static const char file_id[] = "TyError.cc";
/*******************************************************************
This is a replacement version of the methods for the Error class 
in Ptolemy -- link this in instead of Error.cc to go through the
error handler in Tycho instead of the default.

$Id$
Programmer: J. Buck and E. A. Lee

Copyright (c) 1995 The Regents of the University of California.
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

*******************************************************************/

#include <stream.h>
#include "Error.h"
#include "Scheduler.h"
#include "miscFuncs.h"
#include "NamedObj.h"
#include "PtGate.h"
#include "InfString.h"
#include "ptk.h"

// the gate object ensures that messages come out in one piece even
// with multi-threading.

KeptGate gate;

typedef const char cc;

static void outMsg(cc* obj, int warn, cc* m1, cc* m2, cc* m3) {
  CriticalSection region(gate);
  const char* status = warn ? "warning: " : "";
  if (!m2) m2 = "";
  if (!m3) m3 = "";
  InfString msg = status;
  if (obj) msg << obj << ": ";
  msg << m1 << m2 << m3;
  if (Tcl_VarEval(ptkInterp, "ptkImportantMessage .ptkMessage \"", (char*)msg, "\"", NULL)
      != TCL_OK) {
    fprintf(stderr, (char*)msg);
    fflush(stderr);
  }
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
	StringList nam = o.fullName();
//	PigiErrorMark(nam);
	outMsg(nam, 0, m1, m2, m3);
}

void
Error :: warn (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList nam = o.fullName();
//	PigiErrorMark(nam);
	outMsg(nam, 1, m1, m2, m3);
//	FindClear();
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
  if (!m2) m2 = "";
  if (!m3) m3 = "";
  InfString msg;
  if (obj) msg << obj << ": ";
  msg << m1 << m2 << m3;
  if (Tcl_VarEval(ptkInterp, "ptkImportantMessage .ptkMessage \"", (char*)msg, "\"", NULL)
      != TCL_OK) {
    fprintf(stderr, (char*)msg);
    fflush(stderr);
  }
}

void
Error :: message (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
  StringList nam = o.fullName();
  info(nam, m1, m2, m3);
}

void
Error :: message (cc* m1, cc* m2, cc* m3) {
  info(0, m1, m2, m3);
}

// marking is not supported in this implementation
int Error :: canMark() { return 0;}

void Error :: mark(const NamedObj& o) {
//	StringList n = o.fullName();
//	PigiErrorMark(n);
}
