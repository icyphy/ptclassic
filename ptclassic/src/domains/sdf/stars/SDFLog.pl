defstar {
	name { Log }
	domain { SDF } 
	desc { Output the natural logarithm of the input value. }
	version {@(#)SDFLog.pl	1.11 10/01/96}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
Outputs natural logarithm of the input.
If the input is zero or negative, then the run of the universe that
contains this star will be aborted.
<a name="logarithm"></a>
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
