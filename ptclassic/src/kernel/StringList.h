/****************************************************************
SCCS version identification
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
        StringList& operator += (const char*);
	StringList& operator += (char);
	StringList& operator += (int);
	StringList& operator += (unsigned int);
	StringList& operator += (double);
	StringList& operator += (const StringList&);

	// Return first string on list
	const char* head() const {
		return (const char*)SequentialList::head();
	}

	// Return the length in characters.
	int length() const { return totalSize;}
	// Return the number of pieces
	int numPieces() const { return size();}

	// Convert to const char*
	// NOTE!!  This operation modifies the StringList -- it calls
	// the private consolidate method to collect all strings into
	// one string and clean up the garbage.  No modification happens
	// if the StringList is already in one chunk.  A null pointer
	// is always returned if there are no characters, never "".
	operator const char* () { return consolidate();}

	// Make a copy of the StringList as a char* in dynamic memory.
	// the user is responsible for deletion.
	char* newCopy() const;
private:
	// copy constructor body
	void copy(const StringList&);
	// change chunks into one chunk
	const char* consolidate();
	int totalSize;
};

// An Iterator for StringList

class StringListIter : private ListIter {
public:
	StringListIter(const StringList& s) : ListIter(s) {}
	const char* next() { return (const char*)ListIter::next();}
	const char* operator++() { return next();}
	ListIter::reset;
};

// print a StringList on a stream

class ostream;
ostream& operator << (ostream& o,const StringList& sl);

// add objects to a StringList, streamlike syntax

inline StringList& operator << (StringList& list, const char* arg) {
	return list += arg;
}

inline StringList& operator << (StringList& list, char arg) {
	return list += arg;
}

inline StringList& operator << (StringList& list, double arg) {
	return list += arg;
}

inline StringList& operator << (StringList& list, int arg) {
	return list += arg;
}

inline StringList& operator << (StringList& list, const StringList& arg) {
	return list += arg;
}

inline StringList& operator << (StringList& list, unsigned arg) {
	return list += arg;
}

#endif
