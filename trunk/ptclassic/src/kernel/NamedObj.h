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

	// method to print top-level info.  MUST BE REDEFINED
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

#endif
