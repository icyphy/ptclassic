defstar {
	name {Sin}
	domain {SDF}
	desc { This star computes the sine of its input, which is assumed to be an angle in radians.  }
	version {@(#)SDFSin.pl	1.10 3/2/95}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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
		output%0 << sin (double(input%0));
	}
}
