static const char file_id[] = "StringList.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: J. T. Buck, Jose Luis Pino
 (completely replaces a previous version by E. A. Lee and D. G. Messerschmitt)
 Date of creation: 4/22/90

 Methods for the StringList class
*************************************************************************/

#ifdef __GNUG__
#pragma implementation
// implement related InfString class as well.
#pragma implementation "InfString.h"
#endif

#include <ctype.h>
#include <stream.h>
#include "StringList.h"
#include "InfString.h"
#include "miscFuncs.h"

// Must be large enough to hold %d and %u formats (20 digits on 64-bit arch)
#define SMALL_STRING_SIZE 32

// Must be large enough to hold the %.15g format (30 digits on 64-bit arch)
// sign + digit + . + 30 digits + E + sign + 3 digits
#define DOUBLE_STRING_SIZE 48

    
StringList&
StringList :: operator = (const StringList& sl) {
	// check for assignment to self and do nothing
	if (this != &sl) {
		if (size()) initialize ();
		copy(sl);
	}
	return *this;
}

StringList&
StringList :: operator = (const char* s) {
	if (size()) {
		initialize ();
	}
	totalSize = strlen(s);
	put(savestring(s));
	return *this;
}

StringList&
StringList :: operator = (char c) {
	char buf[2];
	buf[0] = c;
	buf[1] = 0;
	return *this = buf;
}

StringList& StringList :: operator = (int i)
{ initialize(); return *this << i;}

StringList& StringList :: operator = (double d)
{ initialize(); return *this << d;}

StringList& StringList :: operator = (unsigned u)
{initialize(); return *this << u;}


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
  if (s) {
    put(savestring(s));
    totalSize+=strlen(s);
  }
  return *this;
}

// Add in a char
StringList&  
StringList :: operator << (char c)
{
        char buf[2];
        buf[0] = c;
        buf[1] = 0;
        return *this << buf;
}

// Add in an int
StringList&  
StringList :: operator << (int i)
{
	char buf[SMALL_STRING_SIZE];
        sprintf(buf, "%d", i); 
	return *this << buf;
}

// Add in an unsigned int
StringList&
StringList :: operator << (unsigned int i)
{
	char buf[SMALL_STRING_SIZE];
        sprintf(buf, "%u", i); 
	return *this << buf;
}

/***********************************************************************

   Add in a double

   @Description Use %.15g as the format string and if the result looks
                like an integer (i.e., there is no decimal point or
                alphabetic characters), append ".0"

***********************************************************************/
StringList&
StringList :: operator << (double f)
{
	char buf[DOUBLE_STRING_SIZE];
        sprintf(buf,"%.15g",f);

	// If the ASCII result looks like an integer, add ".0" so that it
	// doesn't look like an integer anymore.
	int looksLikeInt = 1;
	char *p = buf;
	while (*p) {
		// If there is a decimal point, or an alphabetic
		// character such as the 'e' in scientific notations
		// or the special values "NaN", "Infinity", etc., then
		// it doesn't looks like an integer.
		if ((*p == '.') || isalpha(*p)) {
			looksLikeInt = 0;
			break;
		}
		p++;
	}
	if (looksLikeInt) {
		p[0] = '.';
		p[1] = '0';
		p[2] = 0;
	}
	return *this << buf;
}

// 

/***********************************************************************

   Make a concatenated together copy of the StringList string.

   @Description The user is responsible for deletion.

***********************************************************************/
char*
StringList :: newCopy () const {
	LOG_NEW; char* s = new char[totalSize+1];
	char* out = s;
	StringListIter next(*this);
	const char* p;
	while ((p = next++) != 0) {
		// copy string including terminator
		while ( (*s++ = *p++) );
		// advanced too far; back up by one
		s--;
	}
	return out;
}

/***********************************************************************

   Consolidate all the strings into a single char *

   @Description The memory for the char * is allocated.  This method
   		is used in the cast to const char* and in the chars()
   		method (which gives write access to the store).

***********************************************************************/
char*
StringList :: consolidate () {
	// Handle empty StringList
	if (size() == 0) return 0;

	// If already one segment, handle it w/o work
	if (size() == 1) return (char *)SequentialList::head();
	// Allocate new memory
	char* s = newCopy();

	int temp = totalSize;
	initialize();	// initialize reset totalSize = 0;
	totalSize = temp;

	put(s);
	return s;
}

// Make the stringlist empty
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

/***********************************************************************

  Free all strings in the StringList

  @Description This is a subset of initialize since baseclass will do the rest.

***********************************************************************/
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

// The following function is used for debugging.
// Call them within your debugger using "call" or equivalent.
// If the second argument is nonzero, then it is used as a delimitter
// between elements of the string.

void printStringList(const StringList *s, char* delimitter = 0) {
  StringListIter nexts(*s);
  const char* p;
  while ((p = nexts++) != 0) {
    cout << p;
    if (delimitter) cout << delimitter;
  }
  cout << "\n";
  fflush(stdout);
}

const char* displayStringListItems(const StringList& theList) {
    StringListIter nexts(theList);
    const char* p;
    StringList contents;
    while ((p = nexts++) != 0) {
	contents << p << '\n';
    }	
    return (const char*) contents;
}

const char* displayStringList(const StringList& theList) {
    StringList contents = theList;
    return (const char*) contents;
}
   
