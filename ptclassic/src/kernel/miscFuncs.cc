static const char file_id[] = "miscImpl.cc";
// miscellaneous functions.
// Under g++, inline implementationss for Attribute and the various
// ConstIters classes go here, using the #pragma mechanism.
// $Id$

#ifdef __GNUG__
#pragma implementation
#pragma implementation "Attribute.h"
#pragma implementation "ConstIters.h"

#include "Attribute.h"
#include "ConstIters.h"
#endif

#include "miscFuncs.h"

// This function returns a new, unique temporary file name.
// It lives on the heap and may be deleted when no longer needed.
char* tempFileName() {
	int pid = getpid();
	static int count = 1;
	LOG_NEW; char* buf = new char[17];
	sprintf (buf, "/tmp/pt%04x.%04d", pid, count);
	count++;
	return buf;
}

