static const char file_id[] = "Universe.cc";
/*******************************************************************
SCCS version identification
@(#)Universe.cc	2.15	03/01/97

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
 Programmer: Joseph T. Buck

 Methods for class Universe and Runnable
*******************************************************************/
//
// Print out the universe

#ifdef __GNUG__
#pragma implementation
#endif

#include "Target.h"
#include "Universe.h"
#include "StringList.h"
#include "GalIter.h"
#include "KnownTarget.h"
#include "miscFuncs.h"

Runnable :: Runnable (const char* targetname, const char* dom, Galaxy* g) :
type(dom), galP(g) {
	if (!targetname) targetname = KnownTarget::defaultName(dom);
	pTarget = KnownTarget::clone(targetname);
}

Runnable :: Runnable (Target* tar, const char* dom, Galaxy* g) :
type(dom), galP(g), pTarget(tar) {
	if (!myTarget())
		pTarget = KnownTarget::clone(KnownTarget::defaultName(type));
}

void Runnable :: initTarget() {
	if (myTarget()) {
		if (galP) myTarget()->setGalaxy(*galP);
		myTarget()->initialize();
	}
}

int Runnable :: run() {
	return (myTarget() ? myTarget()->run() : FALSE);
}

StringList Runnable :: displaySchedule() {
	return myTarget()->displaySchedule();
}

// isa
int Runnable :: isA(const char* cname) const {
    return (strcmp(cname,"Runnable") == 0);
}

Scheduler* Universe :: scheduler() const {
	return (myTarget() ? myTarget()->scheduler() : 0);
}

StringList Universe :: print (int recursive) const {
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
	if (myTarget()) myTarget()->setStopTime(stamp) ;
}

// Modify initTarget to invoke begin methods
void Universe :: initTarget() {
	// The following invokes the scheduler
	Runnable::initTarget();

	// The following invokes the begin methods of the stars
	if (myTarget()) myTarget()->begin();
}

// isa
int Universe :: isA(const char* cname) const {
    if (strcmp(cname,"Universe") == 0) {
        return TRUE;
    } else {
        return (Galaxy::isA(cname) || Runnable::isA(cname));
    }
}
