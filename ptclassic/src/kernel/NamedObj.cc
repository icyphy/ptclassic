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
#include <ctype.h>

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

// printing methods.  printVerbose() must always be redefined.
// by default, printRecursive is the same as printVerbose

StringList NamedObj::printRecursive() const { return printVerbose ();}

NamedObj::~NamedObj() {}


// This method creates a name derived from the name of the object
// where all characters in the name that are not alphanumeric are
// changed to '_' so that the resulting name is a legitimate C++
// identifier.  The particular way the name is changed may be different
// in the future, to accomodate future code generators.
StringList NamedObj :: sanitizedName () const {
	const char *s = name;
	char *snm = new char [strlen(s) + 1];
	char *n = snm;
	while (*s != 0) {
	    if(isalnum(*s)) *(n++) = *(s++);
	    else { *(n++) = '_'; s++; }
	}
	*n = 0;
	StringList out(snm);
	delete snm;
	return out;
}

StringList NamedObj :: sanitizedFullName () const {
	StringList out;
	if(blockIamIn != NULL) {
		out = blockIamIn->sanitizedFullName();
		out += ".";
		out += sanitizedName();
	} else {
		out = sanitizedName();
	}
	return out;
}
