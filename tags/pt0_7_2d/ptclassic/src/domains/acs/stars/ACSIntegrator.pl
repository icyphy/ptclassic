defcorona {
	name { Integrator }
	domain { ACS }
	desc {
This is an integrator with leakage and limits.
With the default parameters, input samples are simply accumulated,
and the running sum is the output.  

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
	version { @(#)ACSIntegrator.pl	1.2 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	input {
		name { data }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name {state}
		type {float}
		default {"0.0"}
		desc { An internal state.}
		attributes { A_NONCONSTANT|A_SETTABLE }
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
}
