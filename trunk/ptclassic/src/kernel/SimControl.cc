static const char file_id[] = "SimControl.cc";
/**************************************************************************
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
for example.  The poll flag may be set via a timer, or manually.
Polling functions added by Alan Kamas, 1/95

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#pragma implementation "SimAction.h"
#endif

#include "compat.h"

// Note, SIG_PF should be the same type as SIG_IGN and the same type
//       as the second parameter to signal()
#if !defined(PTIRIX5) 
// PTIRIX5 defines SIG_PF in <signal.h>
#if defined(__GNUG__) || defined(PTHPPA_CFRONT) || defined(PTAIX_XLC)
#if defined (PTSUN4) || defined(PTSOL2)
// sun4, sol2
typedef void (*SIG_PF)();
#else
typedef void (*SIG_PF)(int);
#endif // defined sun4, sol2
#endif // __GNUG__ || PTHPPA_CFRONT || PTAIX_XLC
#endif // !defined PTIRIX5

#if !defined (__GNUG__)
#include <std.h>
#endif

#include "SimAction.h"
#include "SimControl.h"
#include "PtGate.h"
#include "ptsignals.h"
#include <sys/time.h>
#include <signal.h>

// declare action list stuff.
SimActionList* SimControl::preList = 0;
SimActionList* SimControl::postList = 0;
VOLATILE unsigned int SimControl::flags = 0;
VOLATILE int SimControl::pollflag = 0;
int SimControl::nPre = 0, SimControl::nPost = 0;
SimHandlerFunction SimControl::onInt = 0;
SimHandlerFunction SimControl::onPoll = 0;

// magic class: we care only about getting constructor called before
// __main and destructor at end.
class SimControlOwner {
public:
	SimControlOwner() : gk(SimControl::gate) {
		LOG_NEW; SimControl::preList = new SimActionList;
		LOG_NEW; SimControl::postList = new SimActionList;
	}
	~SimControlOwner() {
		LOG_DEL; delete SimControl::preList;
		LOG_DEL; delete SimControl::postList;
	}
private:
	GateKeeper gk;
};

// dummy object causes actions to be called.  It also acts as the
// GateKeeper for SimControl's gate pointer.
static SimControlOwner simControlDummy;

// SimControl's gate pointer
PtGate* SimControl::gate = 0;

int SimControl::internalDoActions(SimActionList* l, Star* which) {
	SimActionListIter nextAction(*l);
	SimAction* a = (SimAction *)NULL;
	while (!haltRequested() && (a = nextAction++) != 0)
		if (a->match(which)) a->apply(which);
	return !haltRequested();
}

SimAction* SimControl::registerAction(SimActionFunction action, int pre,
			  const char* textArg, Star* which) {
	LOG_NEW; SimAction* a = new SimAction(action,textArg,which);
	if (pre) {
		preList->put(a);
		nPre++;
	}
	else {
		postList->put(a);
		nPost++;
	}
	return a;
}

int SimControl::cancel(SimAction* a) {
	int status = FALSE;
	if (preList->member(a)) {
		preList->remove(a);
		status = TRUE;
		nPre--;
	}
	else if (postList->member(a)) {
		postList->remove(a);
		status = TRUE;
		nPost--;
	}
	if (status) {
		LOG_DEL; delete a;
	}
	return status;
}

void SimControl::processFlags() {
	CriticalSection region(gate);
	if ((flags & interrupt) != 0) {
		flags &= ~interrupt;
		// if onInt is set, call the on-interrupt function.
		// optionally set error bit.
		if (onInt && onInt()) 
			flags |= error;
	}
	if (pollflag != 0) {
		// check to see if a polling function is defined
		if (onPoll) {
		    // polling function defined.  Reset the polling
		    // flag only if the polling function has failed
		    // otherwise it is assumed that the polling function
		    // will handle resetting the flag.
		    if (!onPoll()) 
			pollflag = 0;
		} else {
		    // There is no polling function defined.  Reset the flag.
		     pollflag = 0;
		}
	}
}

unsigned int SimControl::readFlags() {
	CriticalSection region(gate);
	return flags;
}

void SimControl::requestHalt () {
	CriticalSection region(gate);
	flags |= halt;
}
 
void SimControl::declareErrorHalt () 
{
	CriticalSection region(gate);
	flags |= (error|halt);
}
	
void SimControl::clearHalt () {
        // Clears all flags
	CriticalSection region(gate);
	flags = 0;
}

SimActionList::SimActionList() {}

SimActionList::~SimActionList() {
	while (size() > 0) {
		SimAction* a = head();
		remove(a);
		LOG_DEL; delete a;
	}
}



// Interrupt handling stuff.  Currently, all interrupts that we catch
// are handled the same way.

void SimControl::intCatcher(int) {
	flags |= interrupt;
	return;
}

/* In gcc2.5.6, signal.h defines SIG_IGN incorrectly. */
#if defined(PTULTRIX) && defined(__GNUG__)
#undef SIG_IGN
#define SIG_IGN         (void (*)(int)) 1
#endif


void SimControl::catchInt(int signo, int always) {
        CriticalSection region(gate);
	// unspecified interrupt means SIGINT.
	if (signo == -1) signo = SIGINT;
	if (!always) {
		// we don't catch signals if they are being ignored now
		SIG_PF tmp = signal(signo, SIG_IGN);
		if (tmp == SIG_IGN) return;
	}
	flags &= ~interrupt;
	signal(signo, (SIG_PF)&SimControl::intCatcher);
}

	// register a function to be called if interrupt flag is set.
	// Returns old handler if any.
SimHandlerFunction SimControl::setInterrupt(SimHandlerFunction f) {
	SimHandlerFunction ret = onInt;
	onInt = f;
	return ret;
}

        // Set the Poll Flag true
void SimControl::setPollFlag() {
        pollflag = 1;
}

        // Get the value of the Poll Flag
        // (turn off signal blocking for a second to allow the signal
        //  to come through if it had been blocked)
int SimControl::getPollFlag() {
	CriticalSection region(gate);  // to make sure blocking restored
	ptReleaseSig(SIGALRM);
        ptBlockSig(SIGALRM);
        return pollflag;
}
	
	// register a function to be called if the poll flag is set.
	// Returns old handler if any.
SimHandlerFunction SimControl::setPollAction(SimHandlerFunction f) {
        CriticalSection region(gate);
	SimHandlerFunction ret = onPoll;
	onPoll = f;
	pollflag = 1;	// Makes sure onPoll can get called 
	return ret;
}

	// use the system timer to set the poll flag at the future time
	// specified by the passed parameters.
	// To make this code as portable as possible, only "basic" system
	// calls are used here.
void SimControl::setPollTimer( int seconds, int micro_seconds ) {
	// reset the timer - this cancels any current timing in progress
        struct itimerval i;
	i.it_interval.tv_sec = i.it_interval.tv_usec = 0;
        i.it_value.tv_sec = seconds;
        i.it_value.tv_usec = micro_seconds;
	// Turn off the poll flag until the timer fires
	pollflag = 0;
	// Turn on the poll flag when the timer expires
	signal(SIGALRM, (SIG_PF)&SimControl::setPollFlag);
	// Make the signal safe from interrupting system calls
        ptSafeSig(SIGALRM);
	// Block the signal so that it will not interrupt system calls
	ptBlockSig(SIGALRM);
	// Start the timer
	setitimer(ITIMER_REAL, &i, 0);
}

