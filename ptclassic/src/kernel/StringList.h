#ifndef _StringList_h
#define _StringList_h 1
#include <std.h>
#include "DataStruct.h"
#include "Output.h"

/****************************************************************
SCCS version identification
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

StringList stores a list of char* strings, and can then read
them back one-by-one, or can concatinate them into a single
char* string. A StringList can only grow; there is no way to
take something away that has been added to the list.

Typical use: For a class that wants to cast into a char*,
the strings can be accumulated into a StringList, and then
let StringList do the cast into char*.

StringList implements a small subset of the function of the
String class

Programmer: D.G. Messerschmitt, U.C. Berkeley
Date: Jan 11, 1990

Revised by: J.T. Buck
The revised version eliminates memory leaks and adds constructors
with arguments.
*******************************************************************/

class StringList : SequentialList
{
public:
	// Constructors
	// Note: StringList foo = bar; calls the constructor,
	// if bar is char*, int, or double
	StringList() {totalSize=0;}

	StringList(const char* s) {
		totalSize=strlen(s);
		char* n = new char[totalSize+1];
		strcpy(n,s);
		put(n);
	}

	StringList(int i) {totalSize=0; *this += i;}

	StringList(double d) {totalSize=0; *this += d;}

	// Copy constructor
	StringList(StringList& s) {totalSize=0; *this += s;}

	// Assignment operator
	StringList& operator = (StringList& sl) {
		// check for assignment to self and do nothing
		if (this != &sl) {
			totalSize = 0;
			initialize ();
			*this += sl;
		}
		return *this;
	}

	// Destructor
	~StringList();

	// Put first string on list: same as +=
	// Use of this operator to add something to a nonempty
	// StringList is deprecated
	StringList& operator = (const char* s) { return *this += s;}
	StringList& operator = (int i) { return *this += i;}
	StringList& operator = (double d) { return *this += d;}

        // Add string to list
        StringList& operator += (const char*);
	StringList& operator += (int);
	StringList& operator += (double);
	StringList& operator += (StringList&);

        // Return size of list
        int size() {return SequentialList::size();}

        // Return next string on list
        char* next () {return (char*) SequentialList::next();}
 
        // Reset to start of the list
        void reset() {SequentialList::reset();}

	// Convert to char*
	// NOTE!!  This operation modifies the StringList -- it calls
	// the private consolidate method to collect all strings into
	// one string and clean up the garbage.
	operator char* () { return consolidate();}

	// The following isn't all that efficient but...
	StringList operator+(char* s) { StringList r(*this); r += s; return r;}

private:
	char* consolidate();
	int totalSize;
};

// print a StringList on a UserOutput
inline UserOutput& operator << (UserOutput& o, StringList& sl) {
	char* s = sl;		// cast
	o << s;
	return o;
}

// print a StringList on a stream (don't give function here so we
// don't have to include stream.h)

class ostream;
ostream& operator << (ostream& o,StringList& sl);

#endif
