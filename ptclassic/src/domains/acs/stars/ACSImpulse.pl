defcorona {
    name { Impulse }
    domain { ACS }
    desc { 
Generate a single impulse or an impulse train.  By default, the
impulse(s) have unity amplitude.  If "period" (default 0) is equal to
zero 0, then only a single impulse is generated; otherwise, it
specifies the period of the impulse train.  The impulse or impulse
train is delayed by the amount specified by "delay".
}
    version { $Id$ }
    author { Eric Pauer }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    output {
        name { output }
        type { float }
    }
    defstate {
	name { period }
	type { int }
	default { 0 }
	desc { The period of the impulse train, 0 = aperiodic. }
    }
    defstate {
	name { delay }
	type { int }
	default { 0 }

	desc { Output will be delayed by this amount (delay must be
        non-negative) }
    } 
    defstate {
	name { count }
	type { int }
	default { 0 }
	desc { An internal state. }
	attributes { A_NONCONSTANT|A_NONSETTABLE }
    }
    setup {
	if (int(period) < 0) {
	  Error::abortRun(*this, "Period must be non-negative");
	}
	if (int(delay) < 0) {
	  Error::abortRun(*this, "Delay must be non-negative");
	}
	if (int(period) == 0) {
	    count = - int(delay);
	}
	else {
	    count = - (int(delay)%int(period));
	}
    }
}
