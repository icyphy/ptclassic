defstar {
	name { Exp }
	domain { SDF } 
	desc { 	Outputs the exponential function of the input. }
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
\fBBugs:\fR Overflow is not handled well.
.Id "exponential"
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
		output%0 << exp(double(input%0));
	}
}
