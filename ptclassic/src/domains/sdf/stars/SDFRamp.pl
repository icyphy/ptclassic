defstar {
	name { Ramp }
	domain { SDF }
	desc {
Generates a ramp signal, starting at "value" (default 0)
with step size "step" (default 1).
	}
	version {$Id$}
	author { D. G. Messerschmitt }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	output {
		name { output }
		type { float }
	}
	defstate {
		name { step }
		type { float }
		default { 1.0 }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { float }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go {
		double t = value;
		output%0 << t;
		t += step;
		value = t;
	}
}
