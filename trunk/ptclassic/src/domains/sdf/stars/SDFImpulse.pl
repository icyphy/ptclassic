defstar {
	name { Impulse }
	domain { SDF }
	desc {
Generate a single impulse or an impulse train.  The impulse(s) have
amplitude "level" (default 1.0).  If "period" (default 0) is equal to 0,
then only a single impulse is generated; otherwise, it specifies the
period of the impulse train.
	}
	version { $Id$ }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
		name { count }
		type { int }
		default { 0 }
		desc { An internal state. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	go {
		double t = 0.0;
		if (int(count) == 0) t = level;
		count = int(count) + 1;
		if (int(period) > 0 && int(count) >= int(period)) count = 0;
		output%0 << t;
	}
}

