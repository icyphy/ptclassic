/**********************************************************************
Version identification:
@(#)SDFTarget.h	1.14     07/21/97

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

 Programmer:  J. T. Buck
 Date of creation: 3/22/92

 Declaration for SDFTarget, the default target to be used in the SDF
 domain.  This used to be declared elsewhere.

 If loopScheduler is 1, a clustering scheduler is used (SDFClustSched),
 otherwise a default scheduler, SDFScheduler, is used.

***********************************************************************/
#ifndef _SDFTarget_h
#define _SDFTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "StringState.h"
#include "IntState.h"
#include "FloatState.h"

// Defined in SDFDomain.cc
extern const char SDFdomainName[];

class SDFTarget : public Target {

public:
	// constructor
	SDFTarget(const char* nam, const char* desc,
		  const char* assocDomain = SDFdomainName);

	// destructor
	~SDFTarget();

	// call the setup method on the galaxy if it exists
	void setup();

	// return a new copy of the target
	Block* makeNew() const;

	// returns TRUE if this is a wormhole with no external inputs
	int selfFiringRequested();

	// returns the no. of inputs to the galaxy/wormhole
	int noOfInputs();

	// If the selfFiringRequested returns true, then this function
        // returns the next self-firing time 
	double nextFiringTime();

protected:
	StringState logFile;
	StringState loopScheduler;

	// schedulePeriod for interface with a timed domain.
	FloatState schedulePeriod;

};
#endif
