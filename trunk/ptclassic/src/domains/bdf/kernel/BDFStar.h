#ifndef _BDFStar_h
#define _BDFStar_h 1
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

 Programmer:  J. T. Buck
 Date of creation: 1/8/91

 BDFStar is a Star that runs under the BDF Scheduler.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "DynDFStar.h"
#include "BDFPortHole.h"
#include "Particle.h"

	////////////////////////////////////
	// class BDFStar
	////////////////////////////////////

class BDFStar : public DynDFStar  {

public:
	BDFStar();

	// my domain
	const char* domain() const;

	// domain specific initialization
	void initialize();

	// system run function
	int run();

	// functions for schedule generation
	int notRunnable();
	int simRunStar(int deferFiring);
	int deferrable();
	// identification
	int isSDFinContext() const;
	int isA(const char*) const;

	// enable dynamic execution
	int setDynamicExecution(int);
	// return value of dyn exec flag.  If true, then star needs to do
	// waitFor calls on conditional inputs.  Otherwise scheduler
	// guarantees that the right data will be present.
	int dynamicExec() const { return dynExec;}

protected:
	// return true if we are in a data-independent context.
	// default implementation just returns the sdf flag.
	virtual int dataIndependent();

	// procedure called if there are not enough tokens on an arc.
	// under the dynamic scheduler, this is normal and we call waitFor
	// to tell the scheduler what we are waiting for more.  with static
	// scheduling we have an error.  So a TRUE return means "OK" and
	// a FALSE return means "error" (e.g. running under static scheduler).
	int handleWait(BDFPortHole& port);

private:
	// boolean flags:
	// I am "SDF in context"
	unsigned char sdfCtx;
	// This star is executing under a dynamic scheduler
	unsigned char dynExec;
};

class BDFStarPortIter : public BlockPortIter {
public:
	BDFStarPortIter(BDFStar& s) : BlockPortIter(s) {}
	BDFPortHole* next() { return (BDFPortHole*)BlockPortIter::next();}
	BDFPortHole* operator++(POSTFIX_OP) { return next();}
};

#endif
