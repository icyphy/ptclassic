defstar {
	name { Exp }
	domain { SDF } 
	desc { 	Outputs the exponential function of the input. }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
\fBBugs:\fR Overflow is not handled well.
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
		float t = input%0;
		output%0 << exp(t);
	}
}
