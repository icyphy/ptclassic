static const char *file_id = "StringList.cc";
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

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include <stream.h>
#include "StringList.h"
#include "UserOutput.h"
#include <string.h>

#define SMALL_STRING 20

// Note: all components of a StringList are in dynamic memory,
// and are deleted by the StringList destructor

// Assignment operator
// The resulting StringList always has 0 or 1 chunk.
StringList&
StringList :: operator = (const StringList& sl) {
	// check for assignment to self and do nothing
	if (this != &sl) {
		if (size()) {
			initialize ();
		}
		totalSize = sl.totalSize;
		if (sl.size()) put(sl.newCopy());
	}
	return *this;
}

// Assignment operator, string argument
StringList&
StringList :: operator = (const char* s) {
	if (size()) {
		initialize ();
	}
	totalSize = strlen(s);
	put(savestring(s));
	return *this;
}

// Assignment operator, char assignment
StringList&
StringList :: operator = (char c) {
	char buf[2];
	buf[0] = c; buf[1] = 0;
	return *this = buf;
}

// Constructors
StringList::StringList(const char* s) {
	totalSize=strlen(s);
	put(savestring(s));
}

StringList::StringList(char c) {totalSize=0; *this += c;}

StringList::StringList(int i) {totalSize=0; *this += i;}

StringList::StringList(double d) {totalSize=0; *this += d;}


// Copy constructor
StringList::StringList (const StringList& s) {
	totalSize = s.totalSize;
	if (s.size()) put(s.newCopy());
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

// Add in a char: uses the above method
StringList&  
StringList :: operator += (char c)
{
        char buf[2];
        buf[0] = c;
        buf[1] = '\0';
        return *this += buf;
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
	if (strchr(buf,'e') == NULL && strchr(buf,'.') == NULL)
		strcat(buf,".0");
	return *this += buf;
}

// Make a new (concatenated together) copy of the StringList string.
char*
StringList :: newCopy () const {
	LOG_NEW; char* s = new char[totalSize+1];
	char* out = s;
	StringListIter next(*this);
	const char* p;
	while ((p = next++) != 0) {
		// copy string including terminator
		while (*s++ = *p++);
		// advanced too far; back up by one
		s--;
	}
	return out;
}

// This method consolidates all strings into one string and returns the
// memory.  It is used in the cast to const char*.
const char*
StringList :: consolidate () {
	// Handle empty StringList
	if (totalSize == 0 || size() == 0)
		return 0;
	// If already one segment, handle it w/o work
	if (size() <= 1) return head();
	// Allocate new memory
	char* s = newCopy();

	int temp = totalSize;
	initialize();	// initialize reset totalSize = 0;
	totalSize = temp;

	put(s);
	return s;
}

// for use in destructor: delete all substrings
// this is also "initialize", in effect.

void StringList::deleteAllStrings() {
	totalSize = 0;
	// note use of ListIter, not StringListIter, so we can delete
	// the result
	ListIter next(*this);
	for (int i=size(); i > 0; i--) {
		char* p = (char*)next++;
		LOG_DEL; delete p;
	}
	SequentialList::initialize();
}

// print a StringList on a UserOutput

UserOutput& operator << (UserOutput& o, const StringList& sl) {
	StringListIter next(sl);
	const char* s;
	while ((s = next++) != 0)
		o << s;
	return o;
}

// print a StringList on a stream

ostream& operator << (ostream&o, const StringList& sl) {
	StringListIter next(sl);
	const char* s;
	while ((s = next++) != 0)
		o << s;
	return o;
}
