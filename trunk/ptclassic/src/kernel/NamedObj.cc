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

#include "NamedObj.h"
#include "StringList.h"
#include "Block.h"

char* NamedObj :: readFullName ()
{
	StringList out;
	if(blockIamIn != NULL) {
	   out = blockIamIn->readFullName();
	   out += ".";
	   out += name;
	   return out;
	} else {
	   return name;
	}
}
