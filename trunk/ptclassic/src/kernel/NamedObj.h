#ifndef _NamedObj_h
#define _NamedObj_h 1

#ifdef __GNUG__
#pragma interface
#endif
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
#include "StringList.h"

class Block;

class NamedObj {
private:
	const char* name;	// my name
	Block* blockIamIn;	// pointer to parent
protected:
	const char* descriptor;	// a descriptor
public:
	// constructors
	NamedObj () : name(""), blockIamIn(0), descriptor("") {}
	NamedObj (const char* n,Block* p,const char* d) :
		name(n), blockIamIn(p), descriptor(d) {}

	virtual const char* readClassName() const;

	// return just the end name
	const char* readName() const { return name;}

	// return the descriptor
	const char* readDescriptor() const {return descriptor;}

	// return the parent block
	Block* parent() const { return blockIamIn;}

	// return the full name
	StringList readFullName() const;

	// set the name and parent
	void setNameParent (const char* my_name,Block* my_parent) {
		name = my_name;
		blockIamIn = my_parent;
	}

	// initialize the object
	virtual void initialize();

	// method to print top-level info.  MUST BE REDEFINED
	virtual StringList printVerbose () const = 0;

	// print all info at all levels.  Same as printVerbose
	// unless redefined
	virtual StringList printRecursive () const;

	// answer queries about my class
	virtual int isA(const char* cname) const;

	// make destructors for all NamedObjs virtual
	virtual ~NamedObj();
};

#include "isa.h"

#endif
