static const char *file_id = "StringList.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer: J. T. Buck
 (completely replaces a previous version by E. A. Lee and D. G. Messerschmitt)
 Date of creation: 4/22/90

 Methods for the StringList class
*************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <stream.h>
#include "StringList.h"
#include "miscFuncs.h"


#define SMALL_STRING 20

// Note: all components of a StringList are in dynamic memory,
// and are deleted by the StringList destructor

// Assignment operator
StringList&
StringList :: operator = (const StringList& sl) {
	// check for assignment to self and do nothing
	if (this != &sl) {
		if (size()) initialize ();
		copy(sl);
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

StringList& StringList :: operator = (int i)
{ initialize(); return *this << i;}

StringList& StringList :: operator = (double d)
{ initialize(); return *this << d;}

StringList& StringList :: operator = (unsigned u)
{initialize(); return *this << u;}


// Constructors
StringList::StringList(const char* s) {
	totalSize=strlen(s);
	put(savestring(s));
}

StringList::StringList(char c) {totalSize=0; *this << c;}

StringList::StringList(int i) {totalSize=0; *this << i;}

StringList::StringList(double d) {totalSize=0; *this << d;}

StringList::StringList(unsigned u) {totalSize=0; *this << u;}

void StringList::copy(const StringList& s) {
	StringListIter nexts(s);
	const char* p;
	while ((p = nexts++) != 0) {
		put(savestring(p));
	}
	totalSize = s.totalSize;
}

// Copy constructor
StringList::StringList (const StringList& s) {
	copy(s);
}

// Add another StringList to the StringList
StringList&
StringList :: operator << (const StringList& l) {
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
StringList :: operator << (const char* s) {
	put(savestring(s));
	totalSize+=strlen(s);
	return *this;
}

// Add in a char: uses the above method
StringList&  
StringList :: operator << (char c)
{
        char buf[2];
        buf[0] = c;
        buf[1] = '\0';
        return *this << buf;
}

// Add in an int: uses the above method
StringList&  
StringList :: operator << (int i)
{
	char buf[SMALL_STRING];
        sprintf(buf,"%d",i); 
	return *this << buf;
}

// Add in an unsigned int
StringList&
StringList :: operator << (unsigned int i)
{
	char buf[SMALL_STRING];
        sprintf(buf,"%u",i); 
	return *this << buf;
}

// Add in a double: uses the above method
StringList&
StringList :: operator << (double f)
{
	char buf[SMALL_STRING];
        sprintf(buf,"%.15g",f);
	if (strchr(buf,'e') == NULL && strchr(buf,'.') == NULL)
		strcat(buf,".0");
	return *this << buf;
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
	if (size() == 0) return 0;

	// If already one segment, handle it w/o work
	if (size() == 1) return head();
	// Allocate new memory
	char* s = newCopy();

	int temp = totalSize;
	initialize();	// initialize reset totalSize = 0;
	totalSize = temp;

	put(s);
	return s;
}

// make the stringlist empty
void StringList::initialize() {
	totalSize = 0;
	// note use of ListIter, not StringListIter, so we can delete
	// the result
	ListIter next(*this);
	for (int i=size(); i > 0; i--) {
		char* p = (char*)next++;
		LOG_DEL; delete [] p;
	}
	SequentialList::initialize();
}

// destructor -- a subset of initialize since baseclass will do the
// rest.
StringList::~StringList() {
	// note use of ListIter, not StringListIter, so we can delete
	// the result
	ListIter next(*this);
	for (int i=size(); i > 0; i--) {
		char* p = (char*)next++;
		LOG_DEL; delete []p;
	}
}

// print a StringList on a stream

ostream& operator << (ostream&o, const StringList& sl) {
	StringListIter next(sl);
	const char* s;
	while ((s = next++) != 0)
		o << s;
	return o;
}
