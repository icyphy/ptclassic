/****************************************************************
SCCS version identification
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

InfString is a string of unbounded size.
Upon casting to (char*), the strings are collapsed into
one continuous string, and a pointer to that string is returned.
The user can treat this as an ordinary pointer to an ordinary
array of characters, and can modify the characters.
But the length of the string should not be changed, and the
InfString destructor is responsible for freeing the allocated memory.

Programmer: E. A. Lee, based on class StringList by Joe Buck

This is almost a verbatim copy of StringList.
This cannot be conveniently derived from StringList without discarding
"const" restrictions and promoting some private members to protected.

WARNING: if a function or expression returns an InfString, and
that value is not assigned to an InfString variable or reference,
and the (char*) cast is used, it is possible (likely under
g++) that the InfString temporary will be destroyed too soon,
leaving the char* pointer pointing to garbage.  Always
assign temporary InfStrings to InfString variables or references
before using the char* conversion.  This includes code like

strcpy(destBuf,functionReturningInfString());

which uses the char* conversion implicitly.

*******************************************************************/

#ifndef _InfString_h
#define _InfString_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "miscFuncs.h"
#include "DataStruct.h"
#include "StringList.h"

class InfString : private SequentialList
{
	friend class InfStringIter;
public:
	// Constructors
	// Note: InfString foo = bar; calls the constructor,
	// if bar is char*, int, or double
	InfString() : totalSize(0) {}
	InfString(char c);
	InfString(const char* s);
	InfString(int i);
	InfString(double d);
	InfString(unsigned u);
	InfString(const StringList& s);

	// Copy constructor
	InfString(const InfString& s);

	// initialize
	void initialize();

	// Assignment operator
	InfString& operator = (const InfString& sl);
	InfString& operator = (const StringList& sl);
	InfString& operator = (const char* s);
	InfString& operator = (char c);
	InfString& operator = (int i);
	InfString& operator = (double d);
	InfString& operator = (unsigned u);

	// Destructor
	~InfString();

        // Add a thing to list
        InfString& operator << (const char*);
	InfString& operator << (char);
	InfString& operator << (int);
	InfString& operator << (unsigned int);
	InfString& operator << (double);
	InfString& operator << (const InfString&);
	InfString& operator << (const StringList&);

	// Return the length in characters.
	int length() const { return totalSize;}

	// Convert to char*
	// NOTE!!  This operation modifies the InfString -- it calls
	// the private consolidate method to collect all strings into
	// one string and clean up the garbage.  No modification happens
	// if the InfString is already in one chunk.  A null pointer
	// is always returned if there are no characters, never "".
	operator char* () { return consolidate();}

	// Make a copy of the InfString as a char* in dynamic memory.
	// the user is responsible for deletion.
	char* newCopy() const;

// add objects to a InfString, old syntax

	InfString& operator += (const char* arg) {
		return *this << arg;
	}

	InfString& operator += (char arg) {
		return *this << arg;
	}

	InfString& operator += (double arg) {
		return *this << arg;
	}

	InfString& operator += (int arg) {
		return *this << arg;
	}

	InfString& operator += (const InfString& arg) {
		return *this << arg;
	}

	InfString& operator += (const StringList& arg) {
		return *this << arg;
	}

	InfString& operator += (unsigned int arg) {
		return *this << arg;
	}

private:
	// Return first string on list
	char* head() const {
		return (char*)SequentialList::head();
	}

	// copy constructor body
	void copy(const InfString&);
	// change chunks into one chunk
	char* consolidate();
	int totalSize;
};

// An Iterator for InfString

class InfStringIter : private ListIter {
public:
	InfStringIter(const InfString& s) : ListIter(s) {}
	char* next() { return (char*)ListIter::next();}
	char* operator++(POSTFIX_OP) { return next();}
	ListIter::reset;
};

// print a InfString on a stream

class ostream;
ostream& operator << (ostream& o,const InfString& sl);

#endif
