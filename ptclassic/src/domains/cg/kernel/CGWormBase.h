#ifndef _CGWormBase_h
#define _CGWormBase_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Wormhole.h"
#include "Profile.h"
#include "MultiTarget.h"
#include "EventHorizon.h"

/*******************************************************************
 SCCS Version identification :
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

 Programmer : Soonhoi Ha
 Date of Creation : 2/21/93
	
********************************************************************/

// CGWormBase can not contain a timed-domain. It may contain another
// type of CGdomain.

	//////////////////////////////
	// CGWormBase
	//////////////////////////////

class CGWormBase : public Wormhole {
public:
	void setup();

	// Constructor
	CGWormBase(Star& s, Galaxy& g, Target* t = 0);
	~CGWormBase();

	// display the schedules of the CGDDFWormholes.
	StringList displaySchedule();

/*******  methods for parallel scheduler. ********/

        // return the profile when "pNum" processors are assigned.
        Profile* getProfile(int pNum); 

	// setup the profile structure before scheduling
	void setupProfile(int n);

        // set the "numProcs" member and initialize Profiles.
        void assignProcs(int num);

        // compute the optimal profile with the given number of processors.
        void computeProfile(int num, int resWork, IntArray* avail);

	// return the profile after scheduling manually the inside domain
	// The argument is the repetition number of the wormhole.
	Profile* manualSchedule(int);

	// final schedule of the inside domain of the wormhole.
	int insideSchedule();

	// down-load the code for the processor based on the invocation number
	// and the profile index.
	void downLoadCode(int, int);

protected: 
	// set outer target
	void setOuterTarget(Target* tp) { outerTarget = tp; }

	// profile
	Profile* myProfile;

	// execution time
	int execTime;

	// myself
	CGWormBase* selfWorm;

private:
        // The total number of processors  in the system.
        int numProcs;
	int prevNum;

	// The final optimal number of assigned processors to inside
	// CG domain.
	int optNum;

	// multi target indicator
	MultiTarget* mtarget;

	// outer target
	Target* outerTarget;

	// Domains to be supported
	StringList supportedDomains;
};

#endif
