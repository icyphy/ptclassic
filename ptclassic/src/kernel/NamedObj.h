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
class Block;

class NamedObj {
private:
	char* name;		// my name
	Block* blockIamIn;	// pointer to parent
protected:
	char* descriptor;	// a descriptor
public:
	NamedObj () {
		name = "noName";
		descriptor = "noDescriptor";      
		blockIamIn = 0;
	}
	char* readName() { return name;}
	char* readDescriptor() {return descriptor;}
	Block* parent() { return blockIamIn;}
	char* readFullName();
	void setNameParent (char* my_name,Block* my_parent) {
		name = my_name;
		blockIamIn = my_parent;
	}
	virtual operator char* () { return "<NamedObj>";}
};

#endif
