defstar {
	name { RampInt }
	domain { SDF }
	desc {
Generates an integer ramp signal, starting at value (default 0)
with step size step (default 1).
	}
	version {$Id$}
	author { D. G. Messerschmitt }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	output {
		name { output }
		type { int }
	}
	defstate {
		name { step }
		type { int }
		default { 1 }
		desc {  Ramp step. }
	}
	defstate {
		name { value }
		type { int }
		default { 0.0 }
		desc {  Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go {
		int t = value;
		output%0 << t;
		t += step;
		value = t;
	}
}
