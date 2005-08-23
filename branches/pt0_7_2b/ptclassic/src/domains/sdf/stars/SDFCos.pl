defstar {
	name {Cos}
	domain {SDF}
	desc { This star computes the cosine of its input, assumed to be an angle in radians.  }
	version {@(#)SDFCos.pl	1.9 3/2/95}
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
	go {
		output%0 << cos (double(input%0));
	}
}
