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

 FloatRamp generates a float ramp function

**************************************************************************/
}
defstar {
	name { FloatRamp }
	domain { SDF }
	desc {
		"Generates a float ramp, starting at value (default 0)\n"
		"with step size step (default 1)"
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { step }
		type { float }
		default { 1.0 }
		desc { "ramp step" }
	}
	defstate {
		name { value }
		type { float }
		default { 0.0 }
		desc { "value output by Ramp" }
	}
	go {
		double t = value;
		output%0 << t;
		t += step;
		value = t;
	}
}
