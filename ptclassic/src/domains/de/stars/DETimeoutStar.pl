defstar
{
    name { TimeoutStar }
    derivedFrom { RepeatStar }
    domain { DE }
    descriptor {
This is the base class for stars that check time-out conditions.
The methods "set", "clear", and "expired" are provided for
setting and testing the timer.
    }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { DE main library }
    explanation {
The \fIset\fP method resets and starts a timer.
Invoking \fIset\fP again will reset the timer and restart it.
Invoking \fIclear\fP stops the timer.
The \fIexpired\fP method indicates whether or not the timer
has reached \fItime-out\fR.
    }

    state {
	name { timeout }
	type { float }
	default { 1.0 }
	descriptor { Duration of the timeout. }
    }

    private {
	int safe : 1;
	int expireFlag : 1;
	double deadline;
    }

    method {
	name { set }
	type { void }
	code
	{
	    safe = FALSE;
	    expireFlag = FALSE;
	    deadline = arrivalTime + timeout;
	    refireAtTime(deadline);
	}
    }

    method {
	name { clear }
	type { void }
	code {
	    safe = TRUE;
	    expireFlag = FALSE;
	}
    }

    method {
	name { expired }
	type { int }
	code {
	    return expireFlag;
	}
    }

    method {
	name { initialize }
	type { void }
	access { public }
	code {
	    clear();
	    DERepeatStar::initialize();
	}
    }

    method {
	name { run }
	type { int }
	access { public }
	code {
	    if (canGetFired()) { 	// time-out event
		if (!safe && arrivalTime == deadline)
		    expireFlag = TRUE;
	    }
	    return DERepeatStar::run();
	}
    }
}
