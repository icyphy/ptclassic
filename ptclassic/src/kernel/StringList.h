/****************************************************************
SCCS version identification
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

StringList stores a list of char* strings, and can then read
them back one-by-one, or can concatinate them into a single
char* string. A StringList can only grow; there is no way to
take something away that has been added to the list.

StringList implements a small subset of the function of the
String class that will someday be an ANSI standard.

Programmer: J. Buck

WARNING: if a function or expression returns a StringList, and
that value is not assigned to a StringList variable or reference,
and the (const char*) cast is used, it is possible (likely under
g++) that the StringList temporary will be destroyed too soon,
leaving the const char* pointer pointing to garbage.  Always
assign temporary StringLists to StringList variables or references
before using the const char* conversion.  This includes code like

strcpy(destBuf,functionReturningStringList());

which uses the const char* conversion implicitly.

*******************************************************************/

#ifndef _StringList_h
#define _StringList_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "miscFuncs.h"
#include "DataStruct.h"

class StringList : private SequentialList
{
	friend class StringListIter;
public:
	// Constructors
	// Note: StringList foo = bar; calls the constructor,
	// if bar is char*, int, or double
	StringList() : totalSize(0) {}

	StringList(char c);

	StringList(const char* s);

	StringList(int i);

	StringList(double d);
	StringList(unsigned u);

	// Copy constructor
	StringList(const StringList& s);

	// initialize
	void initialize();

	// Assignment operator
	StringList& operator = (const StringList& sl);
	StringList& operator = (const char* s);
	StringList& operator = (char c);
	StringList& operator = (int i);
	StringList& operator = (double d);
	StringList& operator = (unsigned u);

	// Destructor
	~StringList();

        // Add a thing to list
        StringList& operator << (const char*);
	StringList& operator << (char);
	StringList& operator << (int);
	StringList& operator << (unsigned int);
	StringList& operator << (double);
	StringList& operator << (const StringList&);

	// Return first string on list
	inline const char* head() const {
		return (const char*)SequentialList::head();
	}

	// Return the length in characters.
	inline int length() const { return totalSize;}
	// Return the number of pieces
	inline int numPieces() const { return size();}

	// Convert to const char*
	// NOTE!!  This operation modifies the StringList -- it calls
	// the private consolidate method to collect all strings into
	// one string and clean up the garbage.  No modification happens
	// if the StringList is already in one chunk.  A null pointer
	// is always returned if there are no characters, never "".
	inline operator const char* () { return consolidate();}

	// Allow write access to the buffer after consolidation.
	// WARNING: this is to permit StringLists to be used with
	// routines that incorrectly request a char* when a const char*
	// would do, or things like Tcl that may temporarily modify
	// the argument but that restore it to its original form before
	// returning.  If used otherwise, no promises are made.
	inline char * chars() { return consolidate();}
	
	// Make a copy of the StringList as a char* in dynamic memory.
	// the user is responsible for deletion.
	char* newCopy() const;

// add objects to a StringList, old syntax

	inline StringList& operator += (const char* arg) {
		return *this << arg;
	}

	inline StringList& operator += (char arg) {
		return *this << arg;
	}

	inline StringList& operator += (double arg) {
		return *this << arg;
	}

	inline StringList& operator += (int arg) {
		return *this << arg;
	}

	inline StringList& operator += (const StringList& arg) {
		return *this << arg;
	}

	inline StringList& operator += (unsigned int arg) {
		return *this << arg;
	}

protected:
	// change chunks into one chunk
	char* consolidate();

private:
	// copy constructor body
	void copy(const StringList&);

	int totalSize;
};

// An Iterator for StringList

class StringListIter : private ListIter {
public:
	StringListIter(const StringList& s) : ListIter(s) {}
	inline const char* next() { return (const char*)ListIter::next();}
	inline const char* operator++(POSTFIX_OP) { return next();}
	ListIter::reset;
};

// print a StringList on a stream

class ostream;
ostream& operator << (ostream& o,const StringList& sl);

#endif
