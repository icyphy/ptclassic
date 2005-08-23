defstar {
	name { Impulse }
	domain { SDF }
	desc {
Generate a single impulse or an impulse train.  The impulse(s) have
amplitude "level" (default 1.0).  If "period" (default 0) is equal to 0,
then only a single impulse is generated; otherwise, it specifies the
period of the impulse train.  The impulse or impulse train is delayed 
by the amount specified by "delay".
	}
	version { @(#)SDFImpulse.pl	2.12	09/13/96 }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	output {
		name { output }
		type { float }
	}
	defstate {
		name { level }
		type { float }
		default { 1.0 }
		desc { The height of the impulse. }
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
		desc {
Output will be delayed by this amount (delay must be non-negative)
		}
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
	go {
		double t = 0.0;
		if (int(count) == 0) t = level;
		count = int(count) + 1;
		if (int(period) > 0 && int(count) >= int(period)) count = 0;
		output%0 << t;
	}
}
