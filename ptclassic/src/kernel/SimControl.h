#ifndef _SimControl_h
#define _SimControl_h 1
#ifdef __GNUG__
#pragma interface
#endif
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


class Star;
class SimAction;
class SimActionList;
typedef void (*SimActionFunction)(Star*,const char*);
typedef int (*SimHandlerFunction)();

class SimControl {
	friend class SimControlOwner;
private:
	// must appear before functions that use it for g++ 2.2's
	// inlining to work correctly.
	static int haltStatus () {
		return (flags & halt) != 0;
	}
public:
	enum flag_bit {
		halt = 1,
		error = 2,
		interrupt = 4,
		poll = 8 };

	static int haltRequested () {
		if (flags != 0) processFlags();
		return haltStatus();
	}

	// Execute all pre-actions for a particular Star.
	// return TRUE if no halting condition arises, FALSE
	// if we are to halt.
	static int doPreActions(Star * which) {
		return (nPre > 0) ? internalDoActions(preList,which)
			: !haltRequested();
	}

	// same, but do postactions.
	static int doPostActions(Star * which) {
		return (nPost > 0) ? internalDoActions(postList,which)
			: !haltRequested();
	}

	// register a preaction or postaction
	// if "pre" is TRUE it is a preaction
	static SimAction* registerAction(SimActionFunction action, int pre,
			       const char* textArg = 0, Star* which = 0);

	// register a function to be called if interrupt flag is set.
	// Returns old handler if any.
	static SimHandlerFunction setInterrupt(SimHandlerFunction f) {
		SimHandlerFunction ret = onInt;
		onInt = f;
		return ret;
	}

	// register a function to be called if the poll flag is set.
	// Returns old handler if any.
	static SimHandlerFunction setPoll(SimHandlerFunction f) {
		SimHandlerFunction ret = onPoll;
		onPoll = f;
		flags |= poll;
		return ret;
	}

	// cancel an action.  Warning: argument is deleted.
	static int cancel(SimAction*);

	static void requestHalt () {
		flags |= halt;
	}
	static void declareErrorHalt () {
		flags |= (error|halt);
	}
	static void clearHalt () {
		flags = 0;
	}

	static unsigned int flagValues() { return flags;}

	static void catchInt(int signo = -1, int always = 0);

private:
	static void intCatcher(int);

	static void processFlags();
	static int internalDoActions(SimActionList*,Star*);

	static SimActionList * preList;
	static SimActionList * postList;
	static unsigned int flags;
	static int nPre, nPost;
	static SimHandlerFunction onInt, onPoll;
};
#endif
