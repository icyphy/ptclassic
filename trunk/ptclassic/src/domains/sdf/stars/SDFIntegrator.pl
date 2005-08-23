defstar {
	name { Integrator }
	domain { SDF }
	desc {
This is an integrator with leakage, limits, and reset.
With the default parameters, input samples are simply accumulated,
and the running sum is the output.  To prevent any resetting in the
middle of a run, connect a Const source with value 0 to the "reset"
input.  Otherwise, whenever a non-zero is received on this input,
the accumulated sum is reset to the current input (i.e. no feedback).

Limits are controlled by the "top" and "bottom" parameters.
If top &gt;= bottom, no limiting is performed (this is the default).
Otherwise, the output is kept between "bottom" and "top".
If "saturate" = YES, saturation is performed.  If "saturate" = NO,
wrap-around is performed (this is the default).
Limiting is performed before output.

Leakage is controlled by the "feedbackGain" state (default 1.0).
The output is the data input plus feedbackGain*state, where state
is the previous output.
	}
	version {@(#)SDFIntegrator.pl	2.18 10/07/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
<a name="filter, integrator"></a>
	}
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
	setup {
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
