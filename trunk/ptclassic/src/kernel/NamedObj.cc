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

StringList NamedObj :: readFullName () const
{
	StringList out;
	if(blockIamIn != NULL) {
		out = blockIamIn->readFullName();
		out += ".";
		out += name;
	} else {
		out = name;
	}
	return out;
}

int NamedObj::isA(const char* cname) const {
	return (strcmp(cname,"NamedObj") == 0);
}

// These virtual functions do nothing in the baseclass; still it
// is more efficient spacewise (and no time penalty) to put them here.
void NamedObj::initialize() {}

const char* NamedObj::readClassName() const {return "NamedObj";}

// printing methods.  printVerbose() must always be redefined.
// by default, printRecursive is the same as printVerbose

StringList NamedObj::printRecursive() const { return printVerbose ();}

NamedObj::~NamedObj() {}

