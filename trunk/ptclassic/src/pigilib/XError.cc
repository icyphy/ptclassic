/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 10/4/90

 This is a replacement version of the methods for the Error class 
 in Ptolemy (the UserOutput class is the same) -- link this in instead
 of Output.cc to get popup windows for error messages.  The
 routine "open_display" must be called first.

 This initial version could use some cleaning up.

*******************************************************************/
extern "C" {
int open_display(const char *);
void close_display();
int accum_string(const char*);
void pr_accum_string();
void clr_accum_string();
void win_msg(const char *);
void close_msg();
int noxquery(const char *);
int query(const char *);
}

#include <stream.h>
#include "Output.h"
#include "Scheduler.h"
#include "miscFuncs.h"
#include "ComplexSubset.h"
#include "NamedObj.h"

// A global output error handler
Error errorHandler;

// Constructor: set the ostream to cerr
Error :: Error () : UserOutput (cerr) {
	clr_accum_string();
}

Error :: Error (ostream& foo) : UserOutput (foo) {}

void
Error :: error() {
	clr_accum_string ();
	accum_string ("ERROR (no message specified)\n");
	pr_accum_string ();
}

void
Error :: error(const char *s) {
	clr_accum_string ();
	accum_string ("ERROR: ");
	accum_string (s);
	pr_accum_string ();
}

void
Error :: error(const char *s, const char *c) {
	clr_accum_string ();
	accum_string ("ERROR: ");
	accum_string (s);
	accum_string (c);
	pr_accum_string ();
}

void
Error :: error (const char *s, int errorCode) {
	char buf[32];
	sprintf (buf, "Error code: %d", errorCode);
	error (s, buf);
}

// print error and halt schedule
void
Error :: abortRun (const char *s) {
	errorHandler.error (s);
	Scheduler::requestHalt();
}

// this one gives the name of the object
void
Error :: abortRun (NamedObj& n, const char *str) {
	StringList s = n.readFullName();
	errorHandler.error (s, str);
	Scheduler::requestHalt();
}

// constructor: output to cout
UserOutput :: UserOutput() : outputStream(&cout), myStream(0) {}

// general constructor
UserOutput :: UserOutput(ostream& foo) : outputStream(&foo), myStream(0) {}

// destructor
UserOutput :: ~UserOutput() {
	// delete the stream only if I created it
	if (myStream)
		delete outputStream;
}

// change the file written to.
void 
UserOutput :: fileName(const char *fileName) {
	if (myStream)
		delete outputStream;
	if ( strcmp(fileName,"cout") == 0 ) {
		outputStream = &cout;
		myStream = FALSE;
	}
	else {
		outputStream = new ostream(expandPathName(fileName),
					   io_writeonly,a_create);
		myStream = TRUE;
	}
}

void
UserOutput :: outputString(const char *s) {
	*outputStream << s;
}

UserOutput&
UserOutput :: operator << (int i) {
	*outputStream << i;
	return *this;
}

UserOutput&
UserOutput :: operator << (const char *s) {
	*outputStream << s;
	return *this;
}

UserOutput&
UserOutput :: operator << (float f) {
	*outputStream << f;
	return *this;
}

UserOutput&
UserOutput :: operator << (const Complex& C) {
        *outputStream << "(" << C.real() << "," << C.imag() << ")";
        return *this;
}

void
UserOutput :: flush () {
	outputStream->flush();
}

// While this function does not belong to the UserOutput or Error
// classes, it's used for stars that do I/O so it is defined here
// (and its prototype is in Output.h)

// This function returns a new, unique temporary file name.
// It lives on the heap and may be deleted when no longer needed.
const char* tempFileName() {
	int pid = getpid();
	static int count = 1;
	char* buf = new char[17];
	sprintf (buf, "/tmp/pt%04x.%04d", pid, count);
	count++;
	return buf;
}
