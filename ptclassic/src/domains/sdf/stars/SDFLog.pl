ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/8/90

 Output is natural log of input.  We abort the run on zero or negative input.

**************************************************************************/
}

defstar {
	name { Log }
	domain { SDF } 
	desc { 	"Outputs natural log of input" }
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
		if (t <= 0) {
			Error::abortRun (*this, ": log of x, x <= 0");
			output%0 << -100.0;
		}
		else output%0 << log(t);
	}
}
