static const char file_id[] = "Universe.cc";
/*******************************************************************
SCCS version identification
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY
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
