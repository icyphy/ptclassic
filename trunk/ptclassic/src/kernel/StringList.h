#ifndef _StringList_h
#define _StringList_h 1
#include "miscFuncs.h"
#include "DataStruct.h"

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

class StringList : public SequentialList
{
	friend class StringListIter;
public:
	// Constructors
	// Note: StringList foo = bar; calls the constructor,
	// if bar is char*, int, or double
	StringList() : totalSize(0) {}

	StringList(const char* s);

	StringList(int i);

	StringList(double d);

	// Copy constructor
	StringList(const StringList& s);

	// Assignment operator
	StringList& operator = (const StringList& sl);
	StringList& operator = (const char* s);

	// Destructor
	~StringList() { deleteAllStrings();}

	// initialize
	void initialize() { deleteAllStrings();}

	// Put number on list: same as +=
	// Use of this operator to add something to a nonempty
	// StringList is deprecated

	StringList& operator = (int i) { return *this += i;}
	StringList& operator = (double d) { return *this += d;}

        // Add string to list
        StringList& operator += (const char*);
	StringList& operator += (int);
	StringList& operator += (double);
	StringList& operator += (const StringList&);

	// Return first string on list
	char* head() const {return (char*)SequentialList::head();}

	// Convert to const char*
	// NOTE!!  This operation modifies the StringList -- it calls
	// the private consolidate method to collect all strings into
	// one string and clean up the garbage.  No modification happens
	// if the StringList is already in one chunk.
	operator const char* () { return consolidate();}

	// Make a copy of the StringList as a char* in dynamic memory.
	// the user is responsible for deletion.
	char* newCopy() const;
private:
	// delete everything currently on the list.  Doesn't zap the
	// underlying SequentialList nodes.
	void deleteAllStrings();

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

// print a StringList on a UserOutput
class UserOutput;
UserOutput& operator << (UserOutput& o, const StringList& sl);

// print a StringList on a stream (don't give function here so we
// don't have to include stream.h)

class ostream;
ostream& operator << (ostream& o,const StringList& sl);

#endif
