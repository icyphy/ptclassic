ident {
// $Id$
// This star produces an impulse or impulse train.
// From the gabriel "impulse" star.
// Programmer: J. Buck
// Date of creation: 5/31
// Converted to use preprocessor: 10/3/90

// Copyright (c) 1990 The Regents of the University of California.
//			All Rights Reserved.
}
defstar {
	name { FloatImpulse }
	domain { SDF }
	desc {
		"Generates an impulse stream of size 'level' (default 1.0).\n"
		"with period 'period' (default 0).\n"
		"If period=0 then only one impulse is generated."
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { level }
		type { float }
		default { 1.0 }
		desc { "height of impulse" }
	}
	defstate {
		name { period }
		type { int }
		default { 0 }
		desc { "period of impulse train, 0 = aperiodic" }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { "internal" }
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

