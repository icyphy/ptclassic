defstar {
	name { Impulse }
	domain { SDF }
	desc {
Generates a stream of impulses of size "level" (default 1.0).
The period is given by "period" (default 0).
If period = 0 then only one impulse is generated.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
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
		if (int(period) > 0 && int(count) >= int(period))
			count = 0;
		output%0 << t;
	}
}

