static const char file_id[] = "XError.cc";
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
	LOG_NEW; char* buf = new char[l];
	if (obj)
		sprintf (buf, "%s%s: %s%s%s", status, obj, m1, m2, m3);
	else
		sprintf (buf, "%s%s%s%s", status, m1, m2, m3);
	PrintErr (buf);
	LOG_NEW; delete buf;
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
	PigiErrorMark(nam);
	outMsg(nam, 0, m1, m2, m3);
}

void
Error :: warn (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	StringList nam = o.fullName();
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
Error :: abortRun (const NamedObj& o, cc* m1, cc* m2, cc* m3) {
	error (o, m1, m2, m3);
	Scheduler::requestHalt();
}

// information messages.
extern "C" {
	void win_msg (const char*);
	int ViGetErrWindows();
	void PrintCon (const char*);
};

static void info(cc* obj, cc* m1, cc* m2, cc* m3) {
	if (!m2) m2 = "";
	if (!m3) m3 = "";
	int l = strlen(m1)+strlen(m2)+strlen(m3)+1;
	if (obj) l += strlen(obj) + 2;
	LOG_NEW; char* buf = new char[l];
	if (obj)
		sprintf (buf, "%s: %s%s%s", obj, m1, m2, m3);
	else	sprintf (buf, "%s%s%s", m1, m2, m3);
	if (ViGetErrWindows())
		win_msg (buf);
	else PrintCon (buf);
	LOG_DEL; delete buf;
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

// marking is supported in this implementation
int Error :: canMark() { return 1;}

void Error :: mark(const NamedObj& o) {
	StringList n = o.fullName();
	PigiErrorMark(n);
}
