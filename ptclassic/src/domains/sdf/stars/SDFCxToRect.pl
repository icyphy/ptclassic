defstar {
	name {CxToRect}
	domain {SDF}
	desc { Convert a complex input to real and imaginary parts. }
	version {@(#)SDFCxToRect.pl	1.12 09/01/97}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name {input}
		type {complex}
	}
	output {
		name {real}
		type {float}
	}
	output {
		name {imag}
		type {float}
	}
	go {
		// We use a temporary variable to avoid gcc2.7.2/2.8 problems
		Complex t = input%0;
		real%0 << t.real();
		imag%0 << t.imag();
	}
}
