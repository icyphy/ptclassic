#include <std.h>
#include "StringList.h"

#define SMALL_STRING 20

// SCCS version identification
// $Id$

// Note: all components of a StringList are in dynamic memory,
// and are deleted by the StringList destructor

// Add another StringList to the StringList
StringList&
StringList :: operator += (StringList& l) {
	l.reset();

	for(int i = l.size(); i > 0; i--) {
		char* s = l.next();
		char* n = new char[strlen(s)+1];
		strcpy(n,s);
		put(n);
	}
	totalSize += l.totalSize;
	return *this;
}

// Add in a char*
StringList&
StringList :: operator += (const char* s) {
	int nadd = strlen(s);
	char* n = new char[nadd+1];
	strcpy(n,s);
	put(n);
	totalSize+=nadd;
	return *this;
}

// Add in an int: uses the above method
StringList&  
StringList :: operator += (int i)
{
	char buf[SMALL_STRING];
        sprintf(buf,"%d",i); 
	return *this += buf;
}

// Add in a double: uses the above method
StringList&
StringList :: operator += (double f)
{
	char buf[SMALL_STRING];
        sprintf(buf,"%g",f);
	return *this += buf;
}

// This method consolidates all strings into one string and returns the
// memory.  It is used in the cast to char*.
char*
StringList :: consolidate () {
	// Handle empty StringList
	if (totalSize == 0)
		return 0;
	// If already one segment, handle it w/o work
	if (size() <= 1) return next();
	// Allocate new memory
	char* s = new char[totalSize+1];
	int totalSoFar = 0;
	reset();
	for(int i = size(); i > 0; i--) {
		char* ss = next();
		strcpy(s+totalSoFar,ss);
		delete ss;
		totalSoFar += strlen(ss);
	}
	initialize();
	put(s);
	totalSize = totalSoFar;
	return s;
}

