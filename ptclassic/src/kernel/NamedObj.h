#ifndef _NamedObj_h
#define _NamedObj_h 1
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
		name(n), blockIamIn(p), descriptor(d){}

	const char* readName() const { return name;}
	const char* readDescriptor() const {return descriptor;}

	Block* parent() const { return blockIamIn;}

	StringList readFullName() const;

	void setNameParent (const char* my_name,Block* my_parent) {
		name = my_name;
		blockIamIn = my_parent;
	}

	virtual operator StringList () = 0;
};

#endif
