/*******************************************************************
SCCS version identification
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.
 Programmer: Joseph T. Buck

 Methods for class Universe and Runnable
*******************************************************************/
//
// Print out the universe

#include "Universe.h"
#include "StringList.h"
#include "GalIter.h"

StringList
Universe :: print (int recursive) const {
	StringList out;
	out = type;
	out += " UNIVERSE: ";
	out += readFullName ();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "CONTENTS:\n";

	if (recursive)
		out += Galaxy::printRecursive();
	else
		out += Galaxy::printVerbose();
	return out;
}

// setting the stopping condition
void Runnable :: setStopTime (float stamp) {
	scheduler->setStopTime(stamp) ;
}

// complete the simulation
void Runnable::wrapupGal (Galaxy& g) {
	GalAllBlockIter next(g);
	Block* b;
	while ((b = next++) != 0) b->wrapup();
}
