/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/16/90

Methods for the KnownBlock class.  See KnownBlock.h for a full
description.

*******************************************************************/
#include "KnownBlock.h"
#include "Output.h"
#include "string.h"
#include "std.h"

extern Error errorHandler;

BlockList *KnownBlock::allstars;	// define the static member

KnownBlock::KnownBlock (Block &block, char *name) {
	static int n_known = 0;

// on the first call, create the known block list.
	if (n_known == 0)
		allstars = new BlockList;
	n_known++;
// set my name and add to the list
	block.setBlock (name,NULL);
	allstars->put (&block);
}

// The main cloner.  This method gives us a new block of the named
// type, by asking the matching block on the list to clone itself.

Block *
KnownBlock::clone(char *type) {
	Block *p = allstars->blockWithName(type);
	if (p) return p->clone();
// If we get here, we don't know the block.  Report error, return NULL.
	errorHandler.error("KnownBlock::clone: unknown star/galaxy name: ",type);
	return 0;
}

// print the known list
void
KnownBlock::printAll (UserOutput &output) {
	Block *p;
	int n = allstars->size ();
	output << "The following " << n << " star/galaxy classes are known:\n";
	for (int i=n; i>0; i--) {
		p = (Block *)allstars->next();
		output << p->readName() << "\n";
	}
}
