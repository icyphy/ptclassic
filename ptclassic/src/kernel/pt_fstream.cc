static const char file_id[] = "pt_fstream.cc";

// Copyright (c) 1992 The Regents of the University of California.
//                       All Rights Reserved.
// Programmer:  J. Buck, Kennard
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

// check for special file names
// This uses the hardcoded descriptors 0,1,2.  These are appropriate
// for UNIX only.
static int check_special(const char *name, int *pNobufB) {
	int	fd = -1, bufB = TRUE;

	/*IF*/ if ( strcmp(name,"<cin>")==0 || strcmp(name,"<stdin>")==0 ) {
	    fd = 0;
	} else if ( strcmp(name,"<cout>")==0 || strcmp(name,"<stdout>")==0 ) {
	    fd = 1;
	    bufB = FALSE;
	} else if ( strcmp(name,"<cerr>")==0 || strcmp(name,"<stderr>")==0 ) {
	    fd = 2;
	    bufB = FALSE;
	} else if ( strcmp(name,"<clog>")==0 || strcmp(name,"<stdlog>")==0 ) {
	    fd = 2;
	}
	if ( pNobufB )
	    *pNobufB = ! bufB;
	return fd;
}

pt_ifstream::pt_ifstream(const char *name,int mode, int prot) {
	open(name,mode,prot);
}

void pt_ifstream::open(const char* name, int mode, int prot) {
	int	nobufB;
	int	fd = check_special(name, &nobufB);
	if ( fd >= 0 )		rdbuf()->attach(fd);
	else			ifstream::open(expand(name),mode,prot);
	if (!*this)	reportError("reading");
	if (nobufB)	rdbuf()->unbuffered(TRUE);
}

pt_ofstream::pt_ofstream(const char *name,int mode, int prot) {
	open(name,mode,prot);
}

void pt_ofstream::open(const char* name, int mode, int prot) {
	int	nobufB;
	int	fd = check_special(name, &nobufB);
	if ( fd >= 0 )		rdbuf()->attach(fd);
	else			ofstream::open(expand(name),mode,prot);
	if (!*this) reportError("writing");
	if (nobufB)	rdbuf()->unbuffered(TRUE);
}
