ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  D. G. Messerschmitt and E. A. Lee
 Date of creation: 1/16/90
 Modified to use states by J. Buck, 5/28/90
 Modified to use preprocessor: 10/3/90, JTB

 IntRamp generates a int ramp function

**************************************************************************/
}
defstar {
	name { IntRamp }
	domain { SDF }
	desc {
		"Generates a integer ramp, starting at value (default 0)\n"
		"with step size step (default 1)"
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { step }
		type { int }
		default { 1 }
		desc { "ramp step" }
	}
	defstate {
		name { value }
		type { int }
		default { 0.0 }
		desc { "initial (or last) value output by Ramp" }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go {
		int t = value;
		output%0 << t;
		t += step;
		value = t;
	}
}
