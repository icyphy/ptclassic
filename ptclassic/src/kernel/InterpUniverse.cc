static const char file_id[] = "InterpUniverse.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  J. Buck

 Methods for InterpUniverse

 An InterpUniverse is an Universe with an interpreted galaxy
 in it.  This is accomplished by making it Runnable and an InterpGalaxy.
 This is the main object that interfaces talk to that makes Ptolemy
 work.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "InterpUniverse.h"
#include "KnownTarget.h"
#include "miscFuncs.h"
#include "SimControl.h"
#include "Target.h"

// constructor: set name and domain
InterpUniverse::InterpUniverse (const char* name, const char* dom) :
Runnable((Target*)0, dom, this), InterpGalaxy(name,dom), targName(0)
{ setBlock(name,NULL);}

const char* InterpUniverse :: targetName() const {
	return targName ? targName : KnownTarget::defaultName(type);
}

int InterpUniverse :: newTarget(const char* newTargName) {
	if (myTarget()) {
		LOG_DEL; delete myTarget();
		setMyTarget(0);
	}
	type = domain();
	targName = newTargName;
	setMyTarget(KnownTarget::clone (targetName()));
	return myTarget() != 0;
}

Scheduler* InterpUniverse :: scheduler() const {
	return myTarget()->scheduler();
}

void InterpUniverse::wrapup() {
	myTarget()->wrapup();
}

InterpUniverse :: ~InterpUniverse() {
	if (!myTarget()) return;
	LOG_DEL; delete myTarget();
	//Is this necessary?
	setMyTarget(0);
}

// Modify initTarget to invoke begin methods
void InterpUniverse :: initTarget() {
        // The following invokes the scheduler
        Runnable::initTarget();
        // The following invokes the begin methods of the stars
        if (!SimControl::haltRequested()) myTarget()->begin();
}

// isa
ISA_FUNC(InterpUniverse,Galaxy);
