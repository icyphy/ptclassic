defstar {
	name { Integrator }
	domain { SDF }
	desc {
Integrator with leakage, limits, and reset.
Leakage is controlled by the "feedbackGain" state (default 1.0).
Limits are controlled by "top" and "bottom".
If top <= bottom, no limiting is performed (default).
If "saturate" = 1, saturation is performed.
If "saturate" = 0, wrap-around is performed (default).
When the "reset" input is non-zero, the Integrator is reset to 0.0.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name { data }
		type { float }
	}
	input {
		name { reset }
		type { int }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name {feedbackGain}
		type {float}
		default {"1.0"}
		desc { The gain on the feedback path.}
	}
	defstate {
		name {top}
		type {float}
		default {"0.0"}
		desc { The upper limit.}
	}
	defstate {
		name {bottom}
		type {float}
		default {"0.0"}
		desc { The lower limit.}
	}
	defstate {
		name {saturate}
		type {int}
		default {"YES"}
		desc { Saturate if YES, wrap around otherwise.}
	}
	defstate {
		name {state}
		type {float}
		default {"0.0"}
		desc { An internal state.}
		attributes { A_NONCONSTANT|A_SETTABLE }
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
