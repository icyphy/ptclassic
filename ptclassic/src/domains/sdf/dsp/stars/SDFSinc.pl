defstar {
	name {Sinc}
	domain {SDF}
	desc {
This star computes the sinc of its input given in radians.
The sinc function is defined as sin(x)/x, with value 1.0 when x = 0.
	}
        version {$Id$}
	author { Brian Evans }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	seealso { Dirichlet RaisedCos }
	input {
		name{input}
		type{float}
		desc{The input x to the sinc function.}
	}
	output {
		name{output}
		type{float}
		desc{The output of the sinc function.}
	}
	ccinclude { <math.h> }
	go {
		const double DELTA = 1.0e-9;	// Approximately zero
		double x = input%0;
		double sincValue;

		if ( ( -DELTA < x ) && ( x < DELTA ) )
		  sincValue = 1.0;
		else
		  sincValue = sin(x) / x;

		output%0 << sincValue;
	}
}
