defstar {
	name { Integrator }
	domain { SDF }
	desc {

An integrator with leakage, limits, and reset.
With the default parameters, input samples are simply accumulated,
and the running sum is the output.  To prevent any resetting in the
middle of a run, connect a d.c. source with value 0.0 to the "reset"
input.  Otherwise, whenever a non-zero is received on this input,
the accumulated sum is reset to the current input (i.e. no feedback).

Limits are controlled by the "top" and "bottom" parameters.
If top <= bottom, no limiting is performed (default).  Otherwise,
the output is kept between "bottom" and "top".  If "saturate" = YES,
saturation is performed.  If "saturate" = NO, wrap-around is performed
(default).  Limiting is performed before output.

Leakage is controlled by the "feedbackGain" state (default 1.0).
The output is the data input plus feedbackGain*state, where state
is the previous output.
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
	    if (int(reset%0) != 0) {
		t = double(data%0);
	    } else {
		t = double(data%0) + double(feedbackGain) * double(state);
	    }
	    if (spread > 0.0) {
		    // Limiting is in effect

		    // Take care of the top
		if (t > double(top))
		    if (int(saturate)) t = double(top);
		    else do t -= spread; while (t > double(top));

		    // Take care of the bottom
		if (t < double(bottom))
		    if (int(saturate)) t = double(bottom);
		    else do t += spread; while (t < double(bottom));
	    }
	    output%0 << t;
	    state = t;
	}
}
