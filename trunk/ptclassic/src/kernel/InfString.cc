static const char *file_id = "InfString.cc";
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

 Programmer: E. A. Lee, based on StringList by J. T. Buck

 Methods for the InfString class
*************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <stream.h>
#include "InfString.h"
#include "miscFuncs.h"


#define SMALL_STRING 32

// Note: all components of a InfString are in dynamic memory,
// and are deleted by the InfString destructor

// Assignment operator
InfString&
InfString :: operator = (const InfString& sl) {
	// check for assignment to self and do nothing
	if (this != &sl) {
		if (size()) initialize ();
		copy(sl);
	}
	return *this;
}

// Assignment operator, StringList argument
InfString&
InfString :: operator = (const StringList& sl) {
	if (size()) {
		initialize ();
	}
	totalSize = sl.length();
	put(sl.newCopy());
	return *this;
}

// Assignment operator, string argument
InfString&
InfString :: operator = (const char* s) {
	if (size()) {
		initialize ();
	}
	totalSize = strlen(s);
	put(savestring(s));
	return *this;
}

// Assignment operator, char assignment
InfString&
InfString :: operator = (char c) {
	char buf[2];
	buf[0] = c; buf[1] = 0;
	return *this = buf;
}

InfString& InfString :: operator = (int i)
{ initialize(); return *this << i;}

InfString& InfString :: operator = (double d)
{ initialize(); return *this << d;}

InfString& InfString :: operator = (unsigned u)
{initialize(); return *this << u;}


// Constructors
InfString::InfString(const char* s) {
	totalSize=strlen(s);
	put(savestring(s));
}

InfString::InfString(char c) {totalSize=0; *this << c;}

InfString::InfString(int i) {totalSize=0; *this << i;}

InfString::InfString(double d) {totalSize=0; *this << d;}

InfString::InfString(unsigned u) {totalSize=0; *this << u;}

InfString::InfString(const StringList& s) {
	put(s.newCopy());
	totalSize = s.length();
}

void InfString::copy(const InfString& s) {
	InfStringIter nexts(s);
	const char* p;
	while ((p = nexts++) != 0) {
		put(savestring(p));
	}
	totalSize = s.totalSize;
}

// Copy constructor
InfString::InfString (const InfString& s) {
	copy(s);
}

// Add another InfString to the InfString
InfString&
InfString :: operator << (const InfString& l) {
// We always add the new argument as a single string, using newCopy.
	if (l.totalSize > 0) {
		char* c = l.newCopy();
		put(c);
		totalSize += l.totalSize;
	}
	return *this;
}

// Add in a StringList
InfString&
InfString :: operator << (const StringList& s) {
	put(s.newCopy());
	totalSize+=s.length();
	return *this;
}

// Add in a char*
InfString&
InfString :: operator << (const char* s) {
	put(savestring(s));
	totalSize+=strlen(s);
	return *this;
}

// Add in a char: uses the above method
InfString&  
InfString :: operator << (char c)
{
        char buf[2];
        buf[0] = c;
        buf[1] = '\0';
        return *this << buf;
}

// Add in an int: uses the above method
InfString&  
InfString :: operator << (int i)
{
	char buf[SMALL_STRING];
        sprintf(buf,"%d",i); 
	return *this << buf;
}

// Add in an unsigned int
InfString&
InfString :: operator << (unsigned int i)
{
	char buf[SMALL_STRING];
        sprintf(buf,"%u",i); 
	return *this << buf;
}

// Add in a double: uses the above method
InfString&
InfString :: operator << (double f)
{
	char buf[SMALL_STRING];
        sprintf(buf,"%.15g",f);
	if (strchr(buf,'e') == NULL && strchr(buf,'.') == NULL)
		strcat(buf,".0");
	return *this << buf;
}

// Make a new (concatenated together) copy of the InfString string.
char*
InfString :: newCopy () const {
	LOG_NEW; char* s = new char[totalSize+1];
	char* out = s;
	InfStringIter next(*this);
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
char*
InfString :: consolidate () {
	// Handle empty InfString
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
void InfString::initialize() {
	totalSize = 0;
	// note use of ListIter, not InfStringIter, so we can delete
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
InfString::~InfString() {
	// note use of ListIter, not InfStringIter, so we can delete
	// the result
	ListIter next(*this);
	for (int i=size(); i > 0; i--) {
		char* p = (char*)next++;
		LOG_DEL; delete []p;
	}
}

// print a InfString on a stream

ostream& operator << (ostream&o, const InfString& sl) {
	InfStringIter next(sl);
	const char* s;
	while ((s = next++) != 0)
		o << s;
	return o;
}
