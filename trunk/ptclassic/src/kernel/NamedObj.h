#ifndef _NamedObj_h
#define _NamedObj_h 1

#ifdef __GNUG__
#pragma interface
#endif
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

 Programmer:  J. T. Buck
 Date of creation: 1/28/90
 Revisions:

A NamedObj is an object that has a name, a descriptor, and a parent,
where the parent is a Block (a specific type of NamedObj).

**************************************************************************/
#include "StringList.h"
#include "isa.h"

class Block;

class NamedObj {
public:
	// constructors
	NamedObj () : nm(""), prnt(0), myDescriptor("") {}
	NamedObj (const char* n,Block* p,const char* d) :
		nm(n), prnt(p), myDescriptor(d) {}

	// class name
	virtual const char* className() const;

	// return just the end name
	const char* name() const { return nm;}

	// return the descriptor
	const char* descriptor() const {return myDescriptor;}

	// return the parent block
	Block* parent() const { return prnt;}

	// return the full name
	virtual StringList fullName() const;

	// set the name and parent
	void setNameParent (const char* my_name,Block* my_parent) {
		nm = my_name;
		prnt = my_parent;
	}

	// initialize the object
	virtual void initialize() = 0;

	// method to print top-level info.
	virtual StringList print (int verbose) const;

	// answer queries about my class
	virtual int isA(const char* cname) const;

	// make destructors for all NamedObjs virtual
	virtual ~NamedObj();
protected:
	void setDescriptor(const char* d) { myDescriptor = d;}
private:
	// name of object
	const char* nm;
	// pointer to its parent
	Block* prnt;
	// descriptor
	const char* myDescriptor;
};

// This structure is a list of NamedObj objects.  It should be privately
// inherited from when creating, say, a list of Block objects.

class NamedObjList : private SequentialList
{
	friend class NamedObjListIter;
	friend class CNamedObjListIter;
public:
	// Add object to list
	void put(NamedObj& s) {SequentialList::put(&s);}

	// export size and initialize functions
	SequentialList::size;
	SequentialList::initialize;

	// find the object with the given name and return pointer
	// const and non-const forms.
	// The rule is that you need a non-const list to get a
	// non-const pointer.

	NamedObj* objWithName(const char* name) {
		return findObj(name);
	}

	const NamedObj* objWithName(const char* name) const {
		return findObj(name);
	}

	// Apply initialize fn to all elements
	void initElements();

	// head of list (const and non-const versions)
	NamedObj* head() {return (NamedObj*)SequentialList::head();}

	const NamedObj* head() const {
		return (const NamedObj*)SequentialList::head();
	}

	// remove obj from list.  Note: obj is not deleted
	int remove(NamedObj* o) { return SequentialList::remove(o);}

	// delete all elements from the list.  WARNING: assumes that the
	// elements are on the heap!
	void deleteAll();
private:
	// publicly, we enforce the rule that you can only get const
	// pointers from a const list.  However, findObj can get non
	// const pointers from a const list; it implements the guts
	// of both objWithName methods.
	NamedObj* findObj(const char* name) const;
};

// an iterator for NamedObjList
class NamedObjListIter : private ListIter {
public:
	NamedObjListIter(NamedObjList& sl) : ListIter (sl) {}
	NamedObj* next() { return (NamedObj*)ListIter::next();}
	NamedObj* operator++(POSTFIX_OP) { return next();}
	ListIter::reset;
};

// an iterator for NamedObjList, const form
class CNamedObjListIter : private ListIter {
public:
	CNamedObjListIter(const NamedObjList& sl) : ListIter (sl) {}
	const NamedObj* next() { return (const NamedObj*)ListIter::next();}
	const NamedObj* operator++(POSTFIX_OP) { return next();}
	ListIter::reset;
};

#endif
