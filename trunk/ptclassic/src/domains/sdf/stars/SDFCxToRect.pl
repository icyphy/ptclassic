defstar {
	name {CxToRect}
	domain {SDF}
	desc { Convert complex data to real and imaginary parts. }
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
		Complex &t = Complex(input%0);
		real%0 << t.real();
		imag%0 << t.imag();
	}
}
