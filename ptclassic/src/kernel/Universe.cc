static const char file_id[] = "Universe.cc";
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "Universe.h"
#include "StringList.h"
#include "GalIter.h"
#include "KnownTarget.h"
#include "miscFuncs.h"

Runnable :: Runnable (const char* targetname, const char* dom, Galaxy* g) :
type(dom), galP(g) {
	if (!targetname) targetname = KnownTarget::defaultName(dom);
	target = KnownTarget::clone(targetname);
}

Runnable :: Runnable (Target* tar, const char* dom, Galaxy* g) :
target(tar), type(dom), galP(g) {
	if (!target)
		target = KnownTarget::clone(KnownTarget::defaultName(type));
}

StringList
Universe :: print (int recursive) const {
	StringList out;
	out = type;
	out += " UNIVERSE: ";
	out += fullName ();
	out += "\n";
	out += "Descriptor: ";
	out += descriptor();
	out += "\n";
	out += "CONTENTS:\n";
	out += Galaxy::print(recursive);
	return out;
}

// setting the stopping condition
void Runnable :: setStopTime (double stamp) {
	target->setStopTime(stamp) ;
}

// isa
ISA_FUNC(Universe,Galaxy);
