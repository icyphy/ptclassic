defstar {
	name {MyStar}
	domain {SDF}
	desc { This star computes the sine of its input (assumed to be an angle in radians) plus 1.0. }
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { $PTOLEMY/src/pigiExample }
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	ccinclude { <math.h> }
	go {
		output%0 << sin (double(input%0)) + 1.0;
	}
}
