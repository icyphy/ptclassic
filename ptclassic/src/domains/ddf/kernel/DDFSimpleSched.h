#ifndef _DDFSimpleSched_h
#define _DDFSimpleSched_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "DDFScheduler.h"
#include "DDFStar.h"
#include "IntState.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  E. A. Lee
 (Based on DDFClustSched, by Soonhoi Ha)

 This is a simple DDF scheduler where
 a basic iteration consists of as many firings as possible, but no
 more than one per actor.  An iteration can consist of more than one
 basic iteration if a pragma is set indicating that a particular star
 should fire some number of times to define an iteration.
 This is meant to replace DynDFScheduler, but for now, inherits
 from it.

**************************************************************************/


	////////////////////////////
	// DDFSimpleSched
	////////////////////////////

class DDFSimpleSched : public DDFScheduler {
public:
	// setup function
	/* virtual */ void setup();

	// run function
	/* virtual */ int run();

	// Turn on or off the feature where the scheduler runs in one
	// iteration until it deadlocks.
	void deadlockIteration(int flag) {runUntilDeadlock = flag;}

	DDFSimpleSched ();

protected:
	// We nullify this routine to avoid the overhead.
	/* virtual */ void initStructures() {};

	// A source is a star with no inputs.
	/* virtual */ int isSource(Star&); 

private:

	// Check the input ports and return the state of the actor.
	// If it is enabled and non-deferrable, return 1.
	// If it is a non-source, enabled and deferrable, return 2.
	// Otherwise, return 0.
	// An actor is deferrable if all its output connections already
	// have enough data to enable the downstream actor.
	int enabledState(DataFlowStar*);

	// Check whether a star is blocked by missing inputs.
	int blockedOnInput(DataFlowStar* );
	
	// check whether output has enough tokens to fire the destination
	// actor. If yes, deferrable.
	int isOutputDeferrable(DataFlowStar*);

	// Check to see whether a firingsPerIteration pragma is registered
	// for the star. If so, parse that pragma and store it in the star.
	// Also, add a pointer to the star to the pragmaStars list
	// the value of the pragma is non-zero.
	int pragmaRegistered(DataFlowStar* as);

	// Fire stars from the sequential list that is passed at most once each.
	// Note that there may be repeated entries in these lists.
	// Return FALSE if an error occurs or a halt is requested.
	// Set the "firedOne" member if any star actually fires.
	int fireStar (DataFlowStar* s);

	// List used to store pointers to the stars that have pragmas
	// registered.  This allows for efficient access at runtime.
	// Note that every element appended to this list must be a
	// DataFlowStar*.
	SequentialList pragmaStars;

	// List of enabled and non-deferrable stars and
	// a pointer to the minimax deferrable star.
        SequentialList enabledNonDef;
	DataFlowStar *enabledDef;

	// Method to put in a star into one of the above lists.
	// It also returns the value returned by enabledState.
        inline int classify (DataFlowStar* c);

	// This flag indicates whether an iteration should consist
	// of running until deadlock.  By default, it is FALSE.
	int runUntilDeadlock;

	// Keep track of whether an actor has fired in the current
	// iteration.
	int firedOne;
	
	// In order to allow for easy profiling (to count the number
	// of stars actually fired by this scheduler, even when there
	// are wormholes), we run the stars through this method.
	inline int runStar (DataFlowStar *s)  {
	  (s->flags[firings])++;
	  return s->run();
	}
	
	// Flags associated with blocks.
	enum {
	  enabled = 0,
	  firings = 1,
	  iter = 2,
	  maxout = 3
	};
};

#endif
