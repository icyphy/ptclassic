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

BlockList *KnownBlock::allBlocks;	// define the static member

int KnownBlock::numBlocks = 0;		// define the number of blocks

// Constructor.  Adds a block to the known list

KnownBlock::KnownBlock (Block &block, const char* name) {
// on the first call, create the known block list.
// It's done this way to get around the order-of-static-constructors problem.
	if (numBlocks == 0)
		allBlocks = new BlockList;
	numBlocks++;
// set my name and add to the list
	block.setBlock (name,NULL);
	allBlocks->put (&block);
}

Block*
KnownBlock::find(const char* type) {
	return numBlocks == 0 ? NULL : allBlocks->blockWithName(type);
}

// The main cloner.  This method gives us a new block of the named
// type, by asking the matching block on the list to clone itself.

Block *
KnownBlock::clone(const char* type) {
	Block *p = find(type);
	if (p) return p->clone();
// If we get here, we don't know the block.  Report error, return NULL.
	errorHandler.error("KnownBlock::clone: unknown star/galaxy name: ",type);
	return 0;
}

// Return known list as text, separated by linefeeds
StringList
KnownBlock::nameList () {
	StringList s;
	if (numBlocks > 0) {
		allBlocks->reset();
		for (int i=numBlocks; i>0; i--) {
			Block& b = (*allBlocks)++;
			s += b.readName();
			s += "\n";
		}
	}
	return s;
}


