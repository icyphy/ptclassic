defstar {
	name { Log }
	domain { SDF } 
	desc { Outputs natural log of input. }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
Outputs natural log of input.
If the input is zero or negative, the run is aborted.
.Id "logarithm"
	}
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	ccinclude { <math.h> }
	go {
		double t = input%0;
		if (t <= 0) {
			Error::abortRun (*this, ": log of x, x <= 0");
			output%0 << -100.0;
		}
		else output%0 << log(t);
	}
}
