defstar {
	name { RampInt }
	domain { SDF }
	desc {
Generate an integer ramp signal, starting at "value" (default 0)
and incrementing by step size "step" (default 1) on each firing.
	}
	version {$Id$}
	author { D. G. Messerschmitt }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	output {
		name { output }
		type { int }
	}
	defstate {
		name { step }
		type { int }
		default { 1 }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { int }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go {
		int t = value;
		output%0 << t;
		t += step;
		value = t;
	}
}
