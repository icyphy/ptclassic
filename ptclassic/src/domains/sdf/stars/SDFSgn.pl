defstar {
	name {Sgn}
	domain {SDF}
	desc {
This star computes the signum of its input.
The output is +/- 1.
Note that 0.0 maps to 1.
	}
	version {@(#)SDFSgn.pl	2.11 10/01/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
<a name="signum"></a>
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{int}
	}
	go {
		double x = input%0;
		int sgn = (x >= 0.0) ? 1 : -1;
		output%0 << sgn;
	}
}
