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

