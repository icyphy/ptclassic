static const char file_id[] = "pt_fstream.cc";

// Copyright (c) 1992 The Regents of the University of California.
//                       All Rights Reserved.
// Programmer:  J. Buck
// $Id$

// This defines derived types of ifstream and ofstream that do the following:

// 1. expand the file name using expandFileName
// 2. report file-open errors using Ptolemy's error-reporting functions.
// The system error value (from "errno") is reported as part of the error
// message and Error::abortRun is called on an open failure.
//
// Otherwise they are the same as their baseclasses.


#include "pt_fstream.h"
#include "miscFuncs.h"
#include "Error.h"
#include <errno.h>
#include "StringList.h"

extern int sys_nerr;
extern char *sys_errlist[];
extern int errno;

// lastName is used to remember the filename for the reporting of errors.
static const char* lastName = 0;

inline const char* expand(const char * name) {
	return lastName = expandPathName(name);
}

// report the reason why the open failed
static void reportError(const char* op) {
	const char * reason = "Unknown error";
	if (errno >= 0 && errno < sys_nerr)
		reason = sys_errlist[errno];
	StringList msg = "Can't open ";
	msg << lastName << " for " << op << ": " << reason;
	Error::abortRun(msg);
}

pt_ifstream::pt_ifstream(const char *name,int mode, int prot)
: ifstream(expand(name),mode,prot)
{
	if (!*this) reportError("reading");
}

void pt_ifstream::open(const char* name, int mode, int prot)
{
	ifstream::open(expand(name),mode,prot);
	if (!*this) reportError("reading");
}

pt_ofstream::pt_ofstream(const char *name,int mode, int prot)
: ofstream(expand(name),mode,prot)
{
	if (!*this) reportError("writing");
}

void pt_ofstream::open(const char* name, int mode, int prot)
{
	ofstream::open(expand(name),mode,prot);
	if (!*this) reportError("writing");
}
