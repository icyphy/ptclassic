/******************************************************************
Version identification:
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

 Programmer:  J. Buck
 Date of creation: 11/29/90

 An InterpUniverse is an Universe with an interpreted galaxy
 in it.  This is accomplished by making it Runnable and an InterpGalaxy.
 This is the main object that interfaces talk to that makes Ptolemy
 work.

 There used to be two slightly different versions of this, one
 in pigiRpc/kernelCalls.h and one in Interpreter.h.  Now there is
 one file.

*******************************************************************/
#ifndef _InterpUniverse_h
#define _InterpUniverse_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Universe.h"
#include "InterpGalaxy.h"
#include "KnownBlock.h"

class InterpUniverse : public InterpGalaxy, public Runnable {
private:
	const char* targName;
public:
	// constructor: args are name and domain
        InterpUniverse (const char* name, const char* dom);
	~InterpUniverse();
	// change the target, by specifying new target name
        int newTarget(const char* newTargName = 0);
	// return my target name
	const char* targetName() const;
	// return my scheduler
	Scheduler* scheduler() const;
	// return my target
	Target* myTarget() const { return target;}
	// run universe
	int run() { return Runnable::run();}
	// wrapup universe
	void wrapup() { target->wrapup();}
	// class identification
	int isA(const char*) const;
};

#endif


