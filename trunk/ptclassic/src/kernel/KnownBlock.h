/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/16/90

The KnownBlock class.

This class provides a list of all known blocks.
The data structure used to reference the list may be
changed.

The idea is that each star or galaxy that is "known to the system"
should add an instance of itself to the known list by code something
like

static MyType proto;
static KnownBlock entry(proto,"MyType");

Then the static method KnownBlock::clone(name) can produce a new
instance of class name.
*******************************************************************/
#ifndef _KnownBlock_h
#define  _KnownBlock_h 1

#include "Block.h"
#include "Galaxy.h"
#include "Output.h"

class KnownBlock {
// The known block list.  It's a pointer only so we can control when
// the constructor is called.  Yuk.
	static BlockList *allstars;
public:
// The constructor takes a block and a name, and adds a corresponding
// entry to the known list.
	KnownBlock (Block &block, const char* name);

// The clone method takes a string, finds the appropriate block in
// allstars, and returns a clone of that block.
	static Block *clone (const char* name);

// method for printing all known blocks.  Maybe should use StringList
// and return a char* instead.
	static void printAll(UserOutput &output);
};

#endif
