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
	StringList() {totalSize=0;}

	StringList(const char* s) {
		totalSize=strlen(s);
		put(savestring(s));
	}

	StringList(int i) {totalSize=0; *this += i;}

	StringList(double d) {totalSize=0; *this += d;}

	// Copy constructor
	StringList(const StringList& s) {
		totalSize = s.totalSize;
		put(s.newCopy());
	}

	// Assignment operator
	StringList& operator = (const StringList& sl);

	// Destructor
	~StringList() { deleteAllStrings(); }

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
	StringList& operator += (const StringList&);

        // Return size of list
        int size() const {return SequentialList::size();}

	// Return first string on list
	char* head() const {return (char*)SequentialList::head();}

	// Convert to char*
	// NOTE!!  This operation modifies the StringList -- it calls
	// the private consolidate method to collect all strings into
	// one string and clean up the garbage.  No modification happens
	// if the StringList is already in one chunk.
	operator char* () { return consolidate();}

	// Make a copy of the StringList as a char* in dynamic memory.
	// the user is responsible for deletion.
	char* newCopy() const;
private:
	// delete everything currently on the list.  Doesn't zap the
	// underlying SequentialList nodes.
	void deleteAllStrings();

	char* consolidate();
	int totalSize;
};

// An Iterator for StringList

class StringListIter : private ListIter {
public:
	StringListIter(const StringList& s) : ListIter(s) {}
	char* next() { return (char*)ListIter::next();}
	char* operator++() { return next();}
	ListIter::reset;
};

// print a StringList on a UserOutput
class UserOutput;
UserOutput& operator << (UserOutput& o, const StringList& sl);

// print a StringList on a stream (don't give function here so we
// don't have to include stream.h)

class ostream;
ostream& operator << (ostream& o,StringList& sl);

#endif
