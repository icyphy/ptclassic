ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer: E. A. Lee
Date of creation: 9/19/90
Modified to use preprocessor: 10/3/90, by JTB

Integrator Star, with leakage, limiting, and reset.

************************************************************************/
}
defstar {
	name { Integrator }
	domain { SDF }
	desc {
	  "Integrator with leakage, limits, and reset.\n"
	  "Leakage is controlled by the feedbackGain state (default 1.0).\n"
	  "Limits are controlled by top and bottom.\n"
	  "If top <= bottom, no limiting is performed (default).\n"
	  "If saturate = 1, saturation is performed.\n"
	  "If saturate = 0, wrap-around is performed (default).\n"
	  "When the reset input is non-zero, the Integrator is reset to 0.0."
	}
	input {
		name { data }
		type { float }
	}
	input {
		name { reset }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name {feedbackGain}
		type {float}
		default {"1.0"}
		desc {"gain on the feedback path"}
	}
	defstate {
		name {top}
		type {float}
		default {"0.0"}
		desc {"upper limit"}
	}
	defstate {
		name {bottom}
		type {float}
		default {"0.0"}
		desc {"lower limit"}
	}
	defstate {
		name {saturate}
		type {int}
		default {"YES"}
		desc {"saturate if true, wrap around otherwise"}
	}
	defstate {
		name {state}
		type {float}
		default {"0.0"}
		desc {"internal state"}
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	protected {
		double spread;
	}
	start {
		spread = double(top) - double(bottom);
	}
	go {
	    double t;
	    if (int(reset%0) != 0) t = 0;
	    else {
		t = float(data%0) + double(feedbackGain) * double(state);
		if (spread > 0.0) {
		    // Limiting is in effect

		    // Take care of the top
		    if (t > double(top))
			if (int(saturate)) t = double(top);
			else do t -= spread;
			     while (t > double(top));

		    // Take care of the bottom
		    if (t < double(bottom))
			if (int(saturate)) t = double(bottom);
			else do t += spread;
			     while (t < double(bottom));
		}
	    }
	    output%0 << t;
	    state = t;
	}
}
