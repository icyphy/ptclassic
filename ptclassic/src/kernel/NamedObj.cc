static const char file_id[] = "NamedObj.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/28/90
 Revisions:

A NamedObj is an object that has a name, a descriptor, and a parent,
where the parent is a Block (a specific type of NamedObj).

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "NamedObj.h"
#include "StringList.h"
#include "Block.h"
#include "SimControl.h"

StringList NamedObj :: fullName () const
{
	StringList out;
	if(prnt != NULL)
		out << prnt->fullName() << ".";
	out << nm;
	return out;
}

int NamedObj :: isA(const char* cname) const
{
	return (strcmp(cname,"NamedObj") == 0);
}

const char* NamedObj :: className() const
{
	return "NamedObj";
}

StringList NamedObj :: print(int) const
{
	StringList out;
	out << fullName() << ": " << className() << "\n";
	return out;
}

// empty destructor.
NamedObj::~NamedObj() {}

// NamedObjList methods

// Find a NamedObj with the given name and return pointer
// This is the guts for both forms of objWithName.
// ListIter is used to bypass the const restrictions we impose.

NamedObj* NamedObjList::findObj(const char* name) const {
	NamedObj *obj;
	ListIter next(*this);
	while ((obj = (NamedObj*)next++) != 0) {
		if (strcmp(name,obj->name()) == 0)
			return obj;
	}
	return 0;
}

// Apply initialize() to all elements.  Stop if an error occurs.

void NamedObjList::initElements() {
	NamedObj *p;
	NamedObjListIter next(*this);
	while ((p = next++) != 0 && !SimControl::haltRequested()) {
		p->initialize();
	};
};

// Delete all elements.  WARNING: elements assumed to be on the heap.

void NamedObjList::deleteAll() {
	NamedObj *p;
	NamedObjListIter next(*this);
	while ((p = next++) != 0) {
		LOG_DEL; delete p;
	}
	initialize();
}
