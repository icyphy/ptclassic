/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 10/4/90

 This is a replacement version of the methods for the Error class 
 in Ptolemy (the UserOutput class is the same) -- link this in instead
 of Output.cc to go through the PrintErr error message handler in pigi.

*******************************************************************/
extern "C" {
void PrintErr(const char*);
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
Error :: Error () : UserOutput (cerr) {}

Error :: Error (ostream& foo) : UserOutput (foo) {}

void
Error :: error() {
	PrintErr("(no message specified)");
}

void
Error :: error(const char *s) {
	PrintErr(s);
}

void
Error :: error(const char *s, const char *c) {
	char* buf = new char[strlen(s)+strlen(c)+1];
	strcpy (buf, s);
	strcat (buf, c);
	PrintErr (buf);
	delete buf;
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
