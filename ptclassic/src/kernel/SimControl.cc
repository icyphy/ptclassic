static const char file_id[] = "SimControl.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

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
for example.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#pragma implementation "SimAction.h"
#endif

// the following should really, I suppose, be #ifdef cfront
#ifndef __GNUG__
#define SignalHandler SIG_PF
#define SignalIgnore SIG_IGN
#else
#ifdef hpux
// on hp-ux, use pre-installed signal.h on gnu
typedef void(*SignalHandler)(int);
#define SignalIgnore SIG_IGN
#endif
#endif

#include "SimAction.h"
#include "SimControl.h"
#include <signal.h>

SimActionList* SimControl::preList = 0;
SimActionList* SimControl::postList = 0;
unsigned int SimControl::flags = 0;
int SimControl::nPre = 0, SimControl::nPost = 0;
SimHandlerFunction SimControl::onInt = 0;
SimHandlerFunction SimControl::onPoll = 0;

class SimControlOwner {
public:
	SimControlOwner() {
		LOG_NEW; SimControl::preList = new SimActionList;
		LOG_NEW; SimControl::postList = new SimActionList;
	}
	~SimControlOwner() {
		LOG_DEL; delete SimControl::preList;
		LOG_DEL; delete SimControl::postList;
	}
};

static SimControlOwner simControlDummy;

int SimControl::internalDoActions(SimActionList* l, Star* which) {
	SimActionListIter nextAction(*l);
	SimAction* a;
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
	int status;
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
	int status;
	if ((flags & interrupt) != 0) {
		flags &= ~interrupt;
		if (onInt) status = onInt();
		if (status) flags |= error;
	}
	if ((flags & poll) != 0) {
		if (onPoll) status = onPoll();
		if (!status) flags &= ~poll;
	}
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

void SimControl::catchInt(int signo, int always) {
	// unspecified interrupt means SIGINT.
	if (signo == -1) signo = SIGINT;
	if (!always) {
		// we don't catch signals if they are being ignored now
		SignalHandler tmp = signal(signo, SignalIgnore);
		if (tmp == SignalIgnore) return;
	}
	flags &= ~interrupt;
	signal(signo, (SignalHandler)SimControl::intCatcher);
}

