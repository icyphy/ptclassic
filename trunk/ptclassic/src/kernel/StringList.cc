/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck
 (completely replaces a previous version by E. A. Lee and D. G. Messerschmitt)
 Date of creation: 4/22/90

 Methods for the StringList class
*************************************************************************/

#include <std.h>
#include <stream.h>
#include "StringList.h"

#define SMALL_STRING 20

// Note: all components of a StringList are in dynamic memory,
// and are deleted by the StringList destructor

// Assignment operator
StringList&
StringList :: operator = (const StringList& sl) {
	// check for assignment to self and do nothing
	if (this != &sl) {
		if (size()) {
			deleteAllStrings();
			initialize ();
		}
		totalSize = sl.totalSize;
		put(sl.newCopy());
	}
	return *this;
}

// Add another StringList to the StringList
StringList&
StringList :: operator += (const StringList& l) {
// We always add the new argument as a single string, using newCopy.
	if (l.totalSize > 0) {
		char* c = l.newCopy();
		put(c);
		totalSize += l.totalSize;
	}
	return *this;
}

// Add in a char*
StringList&
StringList :: operator += (const char* s) {
	put(savestring(s));
	totalSize+=strlen(s);
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

// Make a new (concatenated together) copy of the StringList string.
char*
StringList :: newCopy () const {
	char* s = new char[totalSize+1];
	char* out = s;
	StringListIter next(*this);
	char* p;
	while ((p = next++) != 0) {
		// copy string including terminator
		while (*s++ = *p++);
		// advanced too far; back up by one
		s--;
	}
	return out;
}

// This method consolidates all strings into one string and returns the
// memory.  It is used in the cast to char*.
char*
StringList :: consolidate () {
	// Handle empty StringList
	if (totalSize == 0 || size() == 0)
		return 0;
	// If already one segment, handle it w/o work
	if (size() <= 1) return head();
	// Allocate new memory
	char* s = newCopy();
	deleteAllStrings();
	initialize();
	put(s);
	return s;
}

// for use in destructor: delete all substrings
// we don't delete the nodes because the base class destructor does that.

void StringList::deleteAllStrings() {
	totalSize = 0;
	StringListIter next(*this);
	for (int i=size(); i > 0; i--) {
		delete next++;
	}
}

// print a StringList on a UserOutput

UserOutput& operator << (UserOutput& o, const StringList& sl) {
	StringListIter next(sl);
	char* s;
	while ((s = next++) != 0)
		o << s;
	return o;
}

// print a StringList on a stream

ostream& operator << (ostream&o, StringList& sl) {
	StringListIter next(sl);
	char* s;
	while ((s = next++) != 0)
		o << s;
	return o;
}
