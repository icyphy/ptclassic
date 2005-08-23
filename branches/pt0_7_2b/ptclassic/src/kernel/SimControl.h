#ifndef _SimControl_h
#define _SimControl_h 1
#ifdef __GNUG__
#pragma interface
#endif

// Needed as "volatile" not defined for all compilers
// took this out as it has not yet been integrated into Ptolemy for builds
// #include "compat.h" 
/**************************************************************************
Version identification:
@(#)SimControl.h	1.16	02/18/97

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

 Programmer: J. Buck  
 Date of creation: 6/23/92

SimControl provides for control of a simulation.  Here are the features
provided:

Handler functions can be registered with SimControl.  There are "preactions"
and "postactions" -- preactions are intended to be processed before the
execution of a star, and postactions are intended to be processed after.
The Star class makes sure to call doPreActions and doPostActions at
appropriate times.

These actions may be unconditional or associated with a particular star.

Other features provided include signal handling and a poll function.
The low-level interrupt catcher simply sets a flag bit.  A function may
be registered to provide for an arbitrary action when this bit is set.

The poll function, if enabled, is called between each action function
and when the flag bits are checked.  It can be used as an X event loop,
for example.  Polling functionality completed by Alan Kamas.  1/95

**************************************************************************/
#if ! defined(VOLATILE)
#if (defined(hpux) || defined(__hpux)) && ! defined(__GNUC__)
#define VOLATILE
#else
#define VOLATILE volatile
#endif /* PTHPPA_CFRONT */
#endif /* ! VOLATILE */


class Star;
class SimAction;
class SimActionList;
class PtGate;

typedef void (*SimActionFunction)(Star*,const char*);
typedef int (*SimHandlerFunction)();

class SimControl {
	friend class SimControlOwner;
public:
	// function to read the flag values.  If threading is on,
	// we need a function call, otherwise it is inline.
	inline static unsigned int flagValues() {
		return gate ? readFlags() : flags;
	}
	// function to read flags with a mutual exclusion region.
	static unsigned int readFlags();
private:
	// must appear before functions that use it for g++ 2.2's
	// inlining to work correctly.
	inline static int haltStatus () {
		return (flagValues() & halt) != 0;
	}
public:
	enum flag_bit {
		halt = 1,
		error = 2,
		interrupt = 4,
		poll = 8,
		abort = 16 };

	inline static int haltRequested () {
		if ( flagValues() | getPollFlag() ) processFlags();
		return haltStatus();
	}

	// Execute all pre-actions for a particular Star.
	// return TRUE if no halting condition arises, FALSE
	// if we are to halt.
        // Must have call to haltRequested(), even though it's expensive,
        // in order to prevent crashes due to unprocessed Error::abortRun
        // calls.
	inline static int doPreActions(Star * which) {
		return (nPre > 0) ? internalDoActions(preList,which)
			: !haltRequested();
	}

	// same, but do postactions.
        // Must have call to haltRequested(), even though it's expensive,
        // in order to prevent crashes due to unprocessed Error::abortRun
        // calls.
	inline static int doPostActions(Star * which) {
		return (nPost > 0) ? internalDoActions(postList,which)
			: !haltRequested();
	}

	// register a preaction or postaction
	// if "pre" is TRUE it is a preaction
	static SimAction* registerAction(SimActionFunction action, int pre,
			       const char* textArg = 0, Star* which = 0);

	// register a function to be called if interrupt flag is set.
	// Returns old handler if any.
	static SimHandlerFunction setInterrupt(SimHandlerFunction f);

        // Set the Poll Flag true
	static void setPollFlag();

        // Get the value of the poll flag 
	static int getPollFlag();

	// register a function to be called if the poll flag is set.
	// Returns old handler if any.
	static SimHandlerFunction setPollAction(SimHandlerFunction f);

	// use the system timer to set the poll flag at the future time
        // specified by the passed parameters.
	static void setPollTimer( int seconds, int micro_seconds );

	// cancel an action.  Warning: argument is deleted.
	static int cancel(SimAction*);

	static void requestHalt ();
	static void declareErrorHalt ();
 	static void declareAbortHalt ();
	static void clearHalt ();

	static void catchInt(int signo = -1, int always = 0);

private:
	static void intCatcher(int);

	static void processFlags();
	static int internalDoActions(SimActionList*,Star*);
	// lists of pre- and post-actions
	static SimActionList * preList;
	static SimActionList * postList;
	// # of pre- and post-actions
	static int nPre, nPost;

	// system status flags
	static VOLATILE unsigned int flags;

	// polling status flag.  NOTE: This is a separate flag from "flags" 
	// to avoid any possible race conditions.  It is int to make sure
	// that it is atomic.
	static VOLATILE int pollflag;
	// PtGate for controlling threaded access to flags
	static PtGate* gate;

	static SimHandlerFunction onInt, onPoll;

};
#endif
